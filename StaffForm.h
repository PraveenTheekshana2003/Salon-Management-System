#pragma once
#include "Theme.h"
#include "Database.h"

public ref class StaffForm : public System::Windows::Forms::Form
{
private:
    TextBox^ txtId;
    TextBox^ txtName;
    ComboBox^ cmbPosition;
    TextBox^ txtPhone;
    TextBox^ txtEmail;
    TextBox^ txtSearch;
    DataGridView^ grid;

public:
    StaffForm()
    {
        Build();
        ClearFields();
        LoadGrid();
    }

private:
    void Build()
    {
        this->Text = L"Salon Zelora - Staff Management";
        this->ClientSize = Theme::Sz(980, 700);
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->BackColor = Theme::SkyLight();

        this->Controls->Add(Theme::MakeHeaderBar(L"STAFF MANAGEMENT - ZELORA"));
        Color navy = Theme::Navy();

        this->Controls->Add(Theme::MakeLabel(L"Staff ID", 40, 102, 11, false, navy));
        txtId = Theme::MakeTextBox(170, 98, 330); this->Controls->Add(txtId);
        this->Controls->Add(Theme::MakeLabel(L"Name", 40, 142, 11, false, navy));
        txtName = Theme::MakeTextBox(170, 138, 330); this->Controls->Add(txtName);
        this->Controls->Add(Theme::MakeLabel(L"Position", 40, 182, 11, false, navy));
        cmbPosition = Theme::MakeCombo(170, 178, 330);
        cmbPosition->DropDownStyle = ComboBoxStyle::DropDown;     // allows typing a custom position
        cmbPosition->Items->Add(L"Hair Stylist");
        cmbPosition->Items->Add(L"Beautician");
        cmbPosition->Items->Add(L"Makeup Artist");
        cmbPosition->Items->Add(L"Nail Technician");
        cmbPosition->Items->Add(L"Receptionist");
        cmbPosition->Items->Add(L"Manager");
        this->Controls->Add(cmbPosition);
        this->Controls->Add(Theme::MakeLabel(L"Phone", 40, 222, 11, false, navy));
        txtPhone = Theme::MakeTextBox(170, 218, 330); this->Controls->Add(txtPhone);
        this->Controls->Add(Theme::MakeLabel(L"Email", 40, 262, 11, false, navy));
        txtEmail = Theme::MakeTextBox(170, 258, 330); this->Controls->Add(txtEmail);

        Button^ btnAdd = Theme::MakeButton(L"Add", 40, 305, 100, 38, Theme::SkyBlue(), Color::White);
        btnAdd->Click += gcnew EventHandler(this, &StaffForm::OnAdd);
        this->Controls->Add(btnAdd);
        Button^ btnUpdate = Theme::MakeButton(L"Update", 150, 305, 100, 38, Theme::SkyBlue(), Color::White);
        btnUpdate->Click += gcnew EventHandler(this, &StaffForm::OnUpdate);
        this->Controls->Add(btnUpdate);
        Button^ btnDelete = Theme::MakeButton(L"Delete", 260, 305, 100, 38, Theme::Danger(), Color::White);
        btnDelete->Click += gcnew EventHandler(this, &StaffForm::OnDelete);
        this->Controls->Add(btnDelete);
        Button^ btnClear = Theme::MakeButton(L"Clear", 370, 305, 100, 38, Theme::Grey(), Color::Black);
        btnClear->Click += gcnew EventHandler(this, &StaffForm::OnClear);
        this->Controls->Add(btnClear);

        this->Controls->Add(Theme::MakePicture(L"Staff.jpg", 540, 95, 400, 248));

        this->Controls->Add(Theme::MakeLabel(L"Search:", 40, 362, 10, true, navy));
        txtSearch = Theme::MakeTextBox(110, 358, 300);
        txtSearch->TextChanged += gcnew EventHandler(this, &StaffForm::OnSearch);
        this->Controls->Add(txtSearch);

        grid = gcnew DataGridView();
        grid->Location = Theme::Pt(40, 395);
        grid->Size = Theme::Sz(900, 285);
        Theme::StyleGrid(grid);
        grid->CellClick += gcnew DataGridViewCellEventHandler(this, &StaffForm::OnGridClick);
        this->Controls->Add(grid);
    }

    void ClearFields()
    {
        txtId->ReadOnly = false;
        txtName->Clear(); txtPhone->Clear(); txtEmail->Clear();
        cmbPosition->Text = L"";
        try { txtId->Text = Database::NextId(L"Staff", L"StaffId", L"S"); }
        catch (Exception^) { txtId->Text = L"S001"; }
        txtName->Focus();
    }

    void LoadGrid()
    {
        try { grid->DataSource = Database::GetStaff(txtSearch->Text->Trim()); }
        catch (Exception^ ex) { Theme::Error(L"Could not load staff.\n\n" + ex->Message); }
    }

    bool CheckInput()
    {
        if (txtId->Text->Trim()->Length == 0) { Theme::Warn(L"Staff ID is required."); txtId->Focus(); return false; }
        if (txtName->Text->Trim()->Length == 0) { Theme::Warn(L"Name is required."); txtName->Focus(); return false; }
        if (cmbPosition->Text->Trim()->Length == 0) { Theme::Warn(L"Position is required."); cmbPosition->Focus(); return false; }
        if (!Theme::IsValidPhone(txtPhone->Text->Trim())) { Theme::Warn(L"Enter a valid phone number (9-15 digits)."); txtPhone->Focus(); return false; }
        String^ em = txtEmail->Text->Trim();
        if (em->Length > 0 && !Theme::IsValidEmail(em)) { Theme::Warn(L"Enter a valid email address."); txtEmail->Focus(); return false; }
        return true;
    }

    Staff^ ReadFields()
    {
        return gcnew Staff(txtId->Text->Trim(), txtName->Text->Trim(), cmbPosition->Text->Trim(),
            txtPhone->Text->Trim(), txtEmail->Text->Trim());
    }

    void OnAdd(Object^ sender, EventArgs^ e)
    {
        if (!CheckInput()) return;
        try
        {
            Staff^ s = ReadFields();
            if (Database::Exists(L"Staff", L"StaffId", s->Id)) { Theme::Warn(L"A staff member with this ID already exists."); return; }
            Database::AddStaff(s);
            Theme::Info(L"Staff member added.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnUpdate(Object^ sender, EventArgs^ e)
    {
        if (!CheckInput()) return;
        try
        {
            Staff^ s = ReadFields();
            if (!Database::Exists(L"Staff", L"StaffId", s->Id)) { Theme::Warn(L"Select an existing staff member from the list first."); return; }
            Database::UpdateStaff(s);
            Theme::Info(L"Staff member updated.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnDelete(Object^ sender, EventArgs^ e)
    {
        String^ id = txtId->Text->Trim();
        try
        {
            if (!Database::Exists(L"Staff", L"StaffId", id)) { Theme::Warn(L"Select a staff member from the list first."); return; }
            if (!Theme::Confirm(L"Delete staff member " + id + L"?\n(Their appointments will remain, with no staff assigned.)")) return;
            Database::DeleteStaff(id);
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
        cmbPosition->Text = Convert::ToString(r->Cells[2]->Value);
        txtPhone->Text = Convert::ToString(r->Cells[3]->Value);
        txtEmail->Text = Convert::ToString(r->Cells[4]->Value);
        txtId->ReadOnly = true;
    }
};
