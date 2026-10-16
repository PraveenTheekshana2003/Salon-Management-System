#pragma once
#include "Theme.h"
#include "Database.h"

public ref class ServiceForm : public System::Windows::Forms::Form
{
private:
    int               selectedId;
    TextBox^ txtId;
    TextBox^ txtName;
    TextBox^ txtDesc;
    NumericUpDown^ nudPrice;
    NumericUpDown^ nudDuration;
    DataGridView^ grid;

public:
    ServiceForm()
    {
        selectedId = 0;
        Build();
        ClearFields();
        LoadGrid();
    }

private:
    void Build()
    {
        this->Text = L"Salon Zelora - Service Management";
        this->ClientSize = Theme::Sz(980, 680);
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->BackColor = Theme::SkyLight();

        this->Controls->Add(Theme::MakeHeaderBar(L"SERVICE MANAGEMENT - ZELORA"));
        Color navy = Theme::Navy();

        this->Controls->Add(Theme::MakeLabel(L"Service ID", 40, 102, 11, false, navy));
        txtId = Theme::MakeTextBox(190, 98, 330);
        txtId->ReadOnly = true;
        this->Controls->Add(txtId);

        this->Controls->Add(Theme::MakeLabel(L"Name", 40, 142, 11, false, navy));
        txtName = Theme::MakeTextBox(190, 138, 330); this->Controls->Add(txtName);

        this->Controls->Add(Theme::MakeLabel(L"Description", 40, 182, 11, false, navy));
        txtDesc = Theme::MakeTextBox(190, 178, 330); this->Controls->Add(txtDesc);

        this->Controls->Add(Theme::MakeLabel(L"Price (LKR)", 40, 222, 11, false, navy));
        nudPrice = gcnew NumericUpDown();
        nudPrice->Location = Theme::Pt(190, 218);
        nudPrice->Width = 330;
        nudPrice->Font = Theme::F(10, false);
        nudPrice->DecimalPlaces = 2;
        nudPrice->Maximum = Decimal(10000000);
        nudPrice->Increment = Decimal(100);
        nudPrice->ThousandsSeparator = true;
        this->Controls->Add(nudPrice);

        this->Controls->Add(Theme::MakeLabel(L"Duration (min)", 40, 262, 11, false, navy));
        nudDuration = gcnew NumericUpDown();
        nudDuration->Location = Theme::Pt(190, 258);
        nudDuration->Width = 330;
        nudDuration->Font = Theme::F(10, false);
        nudDuration->Minimum = Decimal(5);
        nudDuration->Maximum = Decimal(600);
        nudDuration->Increment = Decimal(5);
        this->Controls->Add(nudDuration);

        Button^ btnAdd = Theme::MakeButton(L"Add", 40, 315, 100, 38, Theme::SkyBlue(), Color::White);
        btnAdd->Click += gcnew EventHandler(this, &ServiceForm::OnAdd);
        this->Controls->Add(btnAdd);
        Button^ btnUpdate = Theme::MakeButton(L"Update", 150, 315, 100, 38, Theme::SkyBlue(), Color::White);
        btnUpdate->Click += gcnew EventHandler(this, &ServiceForm::OnUpdate);
        this->Controls->Add(btnUpdate);
        Button^ btnDelete = Theme::MakeButton(L"Delete", 260, 315, 100, 38, Theme::Danger(), Color::White);
        btnDelete->Click += gcnew EventHandler(this, &ServiceForm::OnDelete);
        this->Controls->Add(btnDelete);
        Button^ btnClear = Theme::MakeButton(L"Clear", 370, 315, 100, 38, Theme::Grey(), Color::Black);
        btnClear->Click += gcnew EventHandler(this, &ServiceForm::OnClear);
        this->Controls->Add(btnClear);

        this->Controls->Add(Theme::MakePicture(L"Service.jpg", 570, 95, 370, 258));

        grid = gcnew DataGridView();
        grid->Location = Theme::Pt(40, 385);
        grid->Size = Theme::Sz(900, 270);
        Theme::StyleGrid(grid);
        grid->CellClick += gcnew DataGridViewCellEventHandler(this, &ServiceForm::OnGridClick);
        this->Controls->Add(grid);
    }

    void ClearFields()
    {
        selectedId = 0;
        txtId->Text = L"(auto)";
        txtName->Clear(); txtDesc->Clear();
        nudPrice->Value = Decimal(0);
        nudDuration->Value = Decimal(60);
        txtName->Focus();
    }

    void LoadGrid()
    {
        try
        {
            grid->DataSource = Database::GetServices();
            if (grid->Columns->Count >= 5)
            {
                grid->Columns[0]->HeaderText = L"ID";
                grid->Columns[0]->FillWeight = 25;
                grid->Columns[3]->HeaderText = L"Price (LKR)";
                grid->Columns[3]->DefaultCellStyle->Format = L"N2";
                grid->Columns[4]->HeaderText = L"Duration (min)";
            }
        }
        catch (Exception^ ex) { Theme::Error(L"Could not load services.\n\n" + ex->Message); }
    }

    bool CheckInput()
    {
        if (txtName->Text->Trim()->Length == 0) { Theme::Warn(L"Service name is required."); txtName->Focus(); return false; }
        return true;
    }

    Service^ ReadFields()
    {
        Service^ s = gcnew Service();
        s->Id = selectedId;
        s->Name = txtName->Text->Trim();
        s->Description = txtDesc->Text->Trim();
        s->Price = nudPrice->Value;
        s->DurationMinutes = Convert::ToInt32(nudDuration->Value);
        return s;
    }

    void OnAdd(Object^ sender, EventArgs^ e)
    {
        if (!CheckInput()) return;
        try
        {
            Database::AddService(ReadFields());
            Theme::Info(L"Service added.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnUpdate(Object^ sender, EventArgs^ e)
    {
        if (selectedId == 0) { Theme::Warn(L"Select a service from the list first."); return; }
        if (!CheckInput()) return;
        try
        {
            Database::UpdateService(ReadFields());
            Theme::Info(L"Service updated.");
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnDelete(Object^ sender, EventArgs^ e)
    {
        if (selectedId == 0) { Theme::Warn(L"Select a service from the list first."); return; }
        if (!Theme::Confirm(L"Delete this service?\n(Existing appointments will remain, with no service assigned.)")) return;
        try
        {
            Database::DeleteService(selectedId);
            LoadGrid(); ClearFields();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnClear(Object^ sender, EventArgs^ e) { ClearFields(); }

    void OnGridClick(Object^ sender, DataGridViewCellEventArgs^ e)
    {
        if (e->RowIndex < 0) return;
        DataGridViewRow^ r = grid->Rows[e->RowIndex];
        selectedId = Convert::ToInt32(r->Cells[0]->Value);
        txtId->Text = Convert::ToString(selectedId);
        txtName->Text = Convert::ToString(r->Cells[1]->Value);
        txtDesc->Text = Convert::ToString(r->Cells[2]->Value);
        nudPrice->Value = Convert::ToDecimal(r->Cells[3]->Value);
        nudDuration->Value = Convert::ToDecimal(r->Cells[4]->Value);
    }
};
