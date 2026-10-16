#pragma once
#include "Theme.h"
#include "Database.h"

public ref class CustomerForm : public System::Windows::Forms::Form
{
private:
    TextBox^ txtId;
    TextBox^ txtName;
    TextBox^ txtPhone;
    TextBox^ txtEmail;
    TextBox^ txtSearch;
    DataGridView^ grid;

public:
    CustomerForm()
    {
        Build();
        ClearFields();
        LoadGrid();
    }

private:
    void Build()
    {
        this->Text = L"Salon Zelora - Customer Management";
        this->ClientSize = Theme::Sz(980, 700);
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->BackColor = Theme::SkyLight();

        this->Controls->Add(Theme::MakeHeaderBar(L"CUSTOMER MANAGEMENT - ZELORA"));
        Color navy = Theme::Navy();

        this->Controls->Add(Theme::MakeLabel(L"Customer ID", 40, 104, 11, false, navy));
        txtId = Theme::MakeTextBox(170, 100, 330); this->Controls->Add(txtId);
        this->Controls->Add(Theme::MakeLabel(L"Name", 40, 149, 11, false, navy));
        txtName = Theme::MakeTextBox(170, 145, 330); this->Controls->Add(txtName);
        this->Controls->Add(Theme::MakeLabel(L"Phone", 40, 194, 11, false, navy));
        txtPhone = Theme::MakeTextBox(170, 190, 330); this->Controls->Add(txtPhone);
        this->Controls->Add(Theme::MakeLabel(L"Email", 40, 239, 11, false, navy));
        txtEmail = Theme::MakeTextBox(170, 235, 330); this->Controls->Add(txtEmail);

        Button^ btnAdd = Theme::MakeButton(L"Add", 40, 292, 100, 38, Theme::SkyBlue(), Color::White);
        btnAdd->Click += gcnew EventHandler(this, &CustomerForm::OnAdd);
        this->Controls->Add(btnAdd);
        Button^ btnUpdate = Theme::MakeButton(L"Update", 150, 292, 100, 38, Theme::SkyBlue(), Color::White);
        btnUpdate->Click += gcnew EventHandler(this, &CustomerForm::OnUpdate);
        this->Controls->Add(btnUpdate);
        Button^ btnDelete = Theme::MakeButton(L"Delete", 260, 292, 100, 38, Theme::Danger(), Color::White);
        btnDelete->Click += gcnew EventHandler(this, &CustomerForm::OnDelete);
        this->Controls->Add(btnDelete);
        Button^ btnClear = Theme::MakeButton(L"Clear", 370, 292, 100, 38, Theme::Grey(), Color::Black);
        btnClear->Click += gcnew EventHandler(this, &CustomerForm::OnClear);
        this->Controls->Add(btnClear);

        this->Controls->Add(Theme::MakePicture(L"Customer.jpg", 540, 95, 400, 235));

        this->Controls->Add(Theme::MakeLabel(L"Search:", 40, 352, 10, true, navy));
        txtSearch = Theme::MakeTextBox(110, 348, 300);
        txtSearch->TextChanged += gcnew EventHandler(this, &CustomerForm::OnSearch);
        this->Controls->Add(txtSearch);

        grid = gcnew DataGridView();
        grid->Location = Theme::Pt(40, 385);
        grid->Size = Theme::Sz(900, 295);
        Theme::StyleGrid(grid);
        grid->CellClick += gcnew DataGridViewCellEventHandler(this, &CustomerForm::OnGridClick);
        this->Controls->Add(grid);
    }

    void ClearFields()
    {
        txtId->ReadOnly = false;
        txtName->Clear(); txtPhone->Clear(); txtEmail->Clear();
        try { txtId->Text = Database::NextId(L"Customers", L"CustomerId", L"C"); }
        catch (Exception^) { txtId->Text = L"C001"; }
        txtName->Focus();
    }

    void LoadGrid()
    {
        try { grid->DataSource = Database::GetCustomers(txtSearch->Text->Trim()); }
        catch (Exception^ ex) { Theme::Error(L"Could not load customers.\n\n" + ex->Message); }
    }

    bool CheckInput()
    {
        if (txtId->Text->Trim()->Length == 0) { Theme::Warn(L"Customer ID is required."); txtId->Focus(); return false; }
        if (txtName->Text->Trim()->Length == 0) { Theme::Warn(L"Name is required."); txtName->Focus(); return false; }
        if (!Theme::IsValidPhone(txtPhone->Text->Trim())) { Theme::Warn(L"Enter a valid phone number (9-15 digits)."); txtPhone->Focus(); return false; }
        String^ em = txtEmail->Text->Trim();
        if (em->Length > 0 && !Theme::IsValidEmail(em)) { Theme::Warn(L"Enter a valid email address."); txtEmail->Focus(); return false; }
        return true;
    }

    Customer^ ReadFields()
    {
        return gcnew Customer(txtId->Text->Trim(), txtName->Text->Trim(), txtPhone->Text->Trim(), txtEmail->Text->Trim());
    }

    void OnAdd(Object^ sender, EventArgs^ e)
    {
        if (!CheckInput()) return;
        try
        {
            Customer^ c = ReadFields();
            if (Database::Exists(L"Customers", L"CustomerId", c->Id)) { Theme::Warn(L"A customer with this ID already exists."); return; }
            Database::AddCustomer(c);
            Theme::Info(L"Customer added.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnUpdate(Object^ sender, EventArgs^ e)
    {
        if (!CheckInput()) return;
        try
        {
            Customer^ c = ReadFields();
            if (!Database::Exists(L"Customers", L"CustomerId", c->Id)) { Theme::Warn(L"Select an existing customer from the list first."); return; }
            Database::UpdateCustomer(c);
            Theme::Info(L"Customer updated.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnDelete(Object^ sender, EventArgs^ e)
    {
        String^ id = txtId->Text->Trim();
        try
        {
            if (!Database::Exists(L"Customers", L"CustomerId", id)) { Theme::Warn(L"Select a customer from the list first."); return; }
            if (!Theme::Confirm(L"Delete customer " + id + L"?")) return;
            Database::DeleteCustomer(id);
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnClear(Object^ sender, EventArgs^ e) { ClearFields(); }
    void OnSearch(Object^ sender, EventArgs^ e) { LoadGrid(); }

    void OnGridClick(Object^ sender, DataGridViewCellEventArgs^ e)
    {
        if (e->RowIndex < 0) return;
        DataGridViewRow^ r = grid->Rows[e->RowIndex];
        txtId->Text = Convert::ToString(r->Cells[0]->Value);
        txtName->Text = Convert::ToString(r->Cells[1]->Value);
        txtPhone->Text = Convert::ToString(r->Cells[2]->Value);
        txtEmail->Text = Convert::ToString(r->Cells[3]->Value);
        txtId->ReadOnly = true;     // the ID cannot be changed when editing
    }
};
