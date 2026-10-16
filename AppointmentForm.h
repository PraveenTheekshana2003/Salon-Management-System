#pragma once
#include "Theme.h"
#include "Database.h"
#include "BookingAppointmentForm.h"

// Appointment Management: view / search / filter / book / edit / change status / delete
public ref class AppointmentForm : public System::Windows::Forms::Form
{
private:
    TextBox^ txtSearch;
    ComboBox^ cmbFilter;
    ComboBox^ cmbSetStatus;
    Label^ lblCount;
    DataGridView^ grid;

public:
    AppointmentForm()
    {
        Build();
        LoadGrid();
    }

private:
    void Build()
    {
        this->Text = L"Salon Zelora - Appointment Management";
        this->ClientSize = Theme::Sz(1100, 680);
        this->MinimumSize = Theme::Sz(960, 600);
        this->StartPosition = FormStartPosition::CenterParent;
        this->BackColor = Theme::SkyLight();

        this->Controls->Add(Theme::MakeHeaderBar(L"APPOINTMENT MANAGEMENT - ZELORA"));
        Color navy = Theme::Navy();

        this->Controls->Add(Theme::MakeLabel(L"Search:", 30, 94, 10, true, navy));
        txtSearch = Theme::MakeTextBox(100, 90, 260);
        this->Controls->Add(txtSearch);

        this->Controls->Add(Theme::MakeLabel(L"Status:", 385, 94, 10, true, navy));
        cmbFilter = Theme::MakeCombo(450, 90, 150);
        cmbFilter->Items->Add(L"All");
        cmbFilter->Items->Add(L"Pending");
        cmbFilter->Items->Add(L"Confirmed");
        cmbFilter->Items->Add(L"Completed");
        cmbFilter->Items->Add(L"Cancelled");
        cmbFilter->SelectedIndex = 0;
        this->Controls->Add(cmbFilter);

        Button^ btnNew = Theme::MakeButton(L"New Appointment", 30, 135, 170, 38, Theme::SkyBlue(), Color::White);
        btnNew->Click += gcnew EventHandler(this, &AppointmentForm::OnNew);
        this->Controls->Add(btnNew);

        Button^ btnEdit = Theme::MakeButton(L"Edit Selected", 210, 135, 130, 38, Theme::SkyBlue(), Color::White);
        btnEdit->Click += gcnew EventHandler(this, &AppointmentForm::OnEdit);
        this->Controls->Add(btnEdit);

        Button^ btnDelete = Theme::MakeButton(L"Delete", 350, 135, 110, 38, Theme::Danger(), Color::White);
        btnDelete->Click += gcnew EventHandler(this, &AppointmentForm::OnDelete);
        this->Controls->Add(btnDelete);

        Button^ btnRefresh = Theme::MakeButton(L"Refresh", 470, 135, 110, 38, Theme::SkyDark(), Color::White);
        btnRefresh->Click += gcnew EventHandler(this, &AppointmentForm::OnRefresh);
        this->Controls->Add(btnRefresh);

        this->Controls->Add(Theme::MakeLabel(L"Set status:", 30, 194, 10, true, navy));
        cmbSetStatus = Theme::MakeCombo(120, 190, 150);
        cmbSetStatus->Items->Add(L"Pending");
        cmbSetStatus->Items->Add(L"Confirmed");
        cmbSetStatus->Items->Add(L"Completed");
        cmbSetStatus->Items->Add(L"Cancelled");
        cmbSetStatus->SelectedIndex = 1;
        this->Controls->Add(cmbSetStatus);

        Button^ btnApply = Theme::MakeButton(L"Apply", 280, 188, 100, 34, Theme::SkyBlue(), Color::White);
        btnApply->Click += gcnew EventHandler(this, &AppointmentForm::OnApplyStatus);
        this->Controls->Add(btnApply);

        PictureBox^ pic = Theme::MakePicture(L"Appoinment1.jpg", 690, 80, 380, 150);
        pic->Anchor = Theme::AnchorTopRight();
        this->Controls->Add(pic);

        lblCount = Theme::MakeLabel(L"", 30, 232, 9, false, navy);
        this->Controls->Add(lblCount);

        grid = gcnew DataGridView();
        grid->Location = Theme::Pt(30, 255);
        grid->Size = Theme::Sz(1040, 400);
        grid->Anchor = Theme::AnchorAll();
        Theme::StyleGrid(grid);
        grid->CellDoubleClick += gcnew DataGridViewCellEventHandler(this, &AppointmentForm::OnGridDoubleClick);
        grid->CellFormatting += gcnew DataGridViewCellFormattingEventHandler(this, &AppointmentForm::OnFormat);
        this->Controls->Add(grid);

        // attach filter handlers last so they don't fire during setup
        txtSearch->TextChanged += gcnew EventHandler(this, &AppointmentForm::OnRefresh);
        cmbFilter->SelectedIndexChanged += gcnew EventHandler(this, &AppointmentForm::OnRefresh);
    }

    void LoadGrid()
    {
        try
        {
            String^ st = Convert::ToString(cmbFilter->SelectedItem);
            if (String::IsNullOrEmpty(st)) st = L"All";
            DataTable^ dt = Database::GetAppointments(txtSearch->Text->Trim(), st);
            grid->DataSource = dt;
            if (grid->Columns->Count >= 9)
            {
                grid->Columns[0]->HeaderText = L"ID";
                grid->Columns[0]->FillWeight = 30;
                grid->Columns[1]->HeaderText = L"Customer";
                grid->Columns[2]->HeaderText = L"Email";
                grid->Columns[3]->HeaderText = L"Phone";
                grid->Columns[4]->HeaderText = L"Service";
                grid->Columns[5]->HeaderText = L"Staff";
                grid->Columns[6]->HeaderText = L"Date & Time";
                grid->Columns[6]->DefaultCellStyle->Format = L"yyyy-MM-dd  HH:mm";
                grid->Columns[7]->HeaderText = L"Status";
                grid->Columns[8]->HeaderText = L"Notes";
            }
            lblCount->Text = Convert::ToString(dt->Rows->Count) + L" appointment(s)";
        }
        catch (Exception^ ex)
        {
            Theme::Error(L"Could not load appointments.\n\n" + ex->Message);
        }
    }

    int SelectedId()
    {
        if (grid->SelectedRows->Count == 0)
        {
            Theme::Warn(L"Please select an appointment first.");
            return 0;
        }
        return Convert::ToInt32(grid->SelectedRows[0]->Cells[0]->Value);
    }

    void EditById(int id)
    {
        Appointment^ a = Database::GetAppointment(id);
        if (a == nullptr) { Theme::Warn(L"That appointment no longer exists."); LoadGrid(); return; }
        BookingAppointmentForm^ f = gcnew BookingAppointmentForm(a);
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK) LoadGrid();
    }

    void OnRefresh(Object^ sender, EventArgs^ e) { LoadGrid(); }

    void OnNew(Object^ sender, EventArgs^ e)
    {
        BookingAppointmentForm^ f = gcnew BookingAppointmentForm(nullptr);
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK) LoadGrid();
    }

    void OnEdit(Object^ sender, EventArgs^ e)
    {
        int id = SelectedId();
        if (id > 0) EditById(id);
    }

    void OnGridDoubleClick(Object^ sender, DataGridViewCellEventArgs^ e)
    {
        if (e->RowIndex >= 0) EditById(Convert::ToInt32(grid->Rows[e->RowIndex]->Cells[0]->Value));
    }

    void OnDelete(Object^ sender, EventArgs^ e)
    {
        int id = SelectedId();
        if (id == 0) return;
        if (!Theme::Confirm(L"Delete this appointment permanently?")) return;
        try { Database::DeleteAppointment(id); LoadGrid(); }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnApplyStatus(Object^ sender, EventArgs^ e)
    {
        int id = SelectedId();
        if (id == 0) return;
        try
        {
            Database::UpdateAppointmentStatus(id, Convert::ToString(cmbSetStatus->SelectedItem));
            LoadGrid();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnFormat(Object^ sender, DataGridViewCellFormattingEventArgs^ e)
    {
        if (e->ColumnIndex != 7 || e->Value == nullptr) return;
        String^ s = Convert::ToString(e->Value);
        if (s == L"Confirmed")      e->CellStyle->ForeColor = Color::FromArgb(0, 120, 60);
        else if (s == L"Completed") e->CellStyle->ForeColor = Color::FromArgb(0, 90, 170);
        else if (s == L"Cancelled") e->CellStyle->ForeColor = Theme::Danger();
        else                        e->CellStyle->ForeColor = Color::FromArgb(200, 120, 0);
    }
};
