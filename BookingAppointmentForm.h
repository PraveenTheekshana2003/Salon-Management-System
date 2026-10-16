#pragma once
#include "Theme.h"
#include "Database.h"

// Used for both "Book Appointment" (pass nullptr) and "Edit Appointment" (pass an existing one)
public ref class BookingAppointmentForm : public System::Windows::Forms::Form
{
private:
    Appointment^ existing;
    TextBox^ txtName;
    TextBox^ txtEmail;
    TextBox^ txtPhone;
    ComboBox^ cmbService;
    ComboBox^ cmbStaff;
    DateTimePicker^ dtpTime;
    TextBox^ txtNotes;
    ComboBox^ cmbStatus;

public:
    BookingAppointmentForm(Appointment^ appt)
    {
        existing = appt;
        Build();
        LoadLists();
        if (existing != nullptr) FillFromExisting();
    }

private:
    void Build()
    {
        String^ title = L"Book Appointment";
        if (existing != nullptr) title = L"Edit Appointment";

        this->Text = L"Salon Zelora - " + title;
        this->ClientSize = Theme::Sz(600, 650);
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->MinimizeBox = false;
        this->BackColor = Color::White;

        this->Controls->Add(Theme::MakeHeaderBar(title));

        int y = 95;
        int gap = 52;
        Color navy = Theme::Navy();

        this->Controls->Add(Theme::MakeLabel(L"Customer Name:", 40, y + 4, 10, true, navy));
        txtName = Theme::MakeTextBox(210, y, 340); this->Controls->Add(txtName); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Customer Email:", 40, y + 4, 10, true, navy));
        txtEmail = Theme::MakeTextBox(210, y, 340); this->Controls->Add(txtEmail); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Phone Number:", 40, y + 4, 10, true, navy));
        txtPhone = Theme::MakeTextBox(210, y, 340); this->Controls->Add(txtPhone); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Service:", 40, y + 4, 10, true, navy));
        cmbService = Theme::MakeCombo(210, y, 340); this->Controls->Add(cmbService); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Staff (optional):", 40, y + 4, 10, true, navy));
        cmbStaff = Theme::MakeCombo(210, y, 340); this->Controls->Add(cmbStaff); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Appointed Time:", 40, y + 4, 10, true, navy));
        dtpTime = gcnew DateTimePicker();
        dtpTime->Location = Theme::Pt(210, y);
        dtpTime->Width = 340;
        dtpTime->Font = Theme::F(10, false);
        dtpTime->Format = DateTimePickerFormat::Custom;
        dtpTime->CustomFormat = L"yyyy-MM-dd  HH:mm";
        if (existing == nullptr)
        {
            dtpTime->MinDate = DateTime::Today;
            dtpTime->Value = DateTime::Now.AddHours(1);
        }
        this->Controls->Add(dtpTime); y += gap;

        this->Controls->Add(Theme::MakeLabel(L"Notes:", 40, y + 4, 10, true, navy));
        txtNotes = Theme::MakeTextBox(210, y, 340);
        txtNotes->Multiline = true;
        txtNotes->Height = 70;
        this->Controls->Add(txtNotes); y += 90;

        cmbStatus = Theme::MakeCombo(210, y, 340);
        cmbStatus->Items->Add(L"Pending");
        cmbStatus->Items->Add(L"Confirmed");
        cmbStatus->Items->Add(L"Completed");
        cmbStatus->Items->Add(L"Cancelled");
        cmbStatus->SelectedIndex = 0;
        if (existing != nullptr)
        {
            this->Controls->Add(Theme::MakeLabel(L"Status:", 40, y + 4, 10, true, navy));
            this->Controls->Add(cmbStatus);
        }

        Button^ btnSubmit = Theme::MakeButton(L"Submit", 210, 585, 150, 44, Theme::SkyBlue(), Color::White);
        btnSubmit->Click += gcnew EventHandler(this, &BookingAppointmentForm::OnSubmit);
        this->Controls->Add(btnSubmit);

        Button^ btnCancel = Theme::MakeButton(L"Cancel", 400, 585, 150, 44, Theme::Grey(), Color::Black);
        btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->Controls->Add(btnCancel);
        this->CancelButton = btnCancel;
    }

    void LoadLists()
    {
        try
        {
            DataTable^ sv = Database::GetServices();
            cmbService->DataSource = sv;
            cmbService->DisplayMember = L"Name";
            cmbService->ValueMember = L"ServiceId";

            DataTable^ st = Database::GetStaffList();
            DataRow^ none = st->NewRow();
            none->ItemArray = gcnew array<Object^> { "", "(No preference)" };
            st->Rows->InsertAt(none, 0);
            cmbStaff->DataSource = st;
            cmbStaff->DisplayMember = L"Name";
            cmbStaff->ValueMember = L"StaffId";
            cmbStaff->SelectedIndex = 0;
        }
        catch (Exception^ ex)
        {
            Theme::Error(L"Could not load services/staff.\n\n" + ex->Message);
        }
    }

    void FillFromExisting()
    {
        txtName->Text = existing->CustomerName;
        txtEmail->Text = existing->CustomerEmail;
        txtPhone->Text = existing->Phone;
        txtNotes->Text = existing->Notes;
        dtpTime->Value = existing->AppointmentTime;
        if (existing->ServiceId > 0) cmbService->SelectedValue = existing->ServiceId;
        if (!String::IsNullOrEmpty(existing->StaffId)) cmbStaff->SelectedValue = existing->StaffId;
        int idx = cmbStatus->FindStringExact(existing->Status);
        if (idx >= 0) cmbStatus->SelectedIndex = idx;
    }

    void OnSubmit(Object^ sender, EventArgs^ e)
    {
        String^ name = txtName->Text->Trim();
        String^ email = txtEmail->Text->Trim();
        String^ phone = txtPhone->Text->Trim();

        if (name->Length == 0) { Theme::Warn(L"Please enter the customer name."); txtName->Focus(); return; }
        if (email->Length == 0 || !Theme::IsValidEmail(email)) { Theme::Warn(L"Please enter a valid email address."); txtEmail->Focus(); return; }
        if (!Theme::IsValidPhone(phone)) { Theme::Warn(L"Please enter a valid phone number (9-15 digits)."); txtPhone->Focus(); return; }
        if (cmbService->SelectedValue == nullptr) { Theme::Warn(L"Please select a service."); return; }

        DateTime t = dtpTime->Value;
        t = DateTime(t.Year, t.Month, t.Day, t.Hour, t.Minute, 0);
        if (existing == nullptr && DateTime::Compare(t, DateTime::Now.AddMinutes(-1)) < 0)
        {
            Theme::Warn(L"The appointment time cannot be in the past.");
            return;
        }

        Appointment^ a = gcnew Appointment();
        if (existing != nullptr) a = existing;
        a->CustomerName = name;
        a->CustomerEmail = email;
        a->Phone = phone;
        a->ServiceId = Convert::ToInt32(cmbService->SelectedValue);
        a->StaffId = Convert::ToString(cmbStaff->SelectedValue);
        a->AppointmentTime = t;
        a->Notes = txtNotes->Text->Trim();
        if (existing != nullptr) a->Status = Convert::ToString(cmbStatus->SelectedItem); else a->Status = L"Pending";

        try
        {
            if (existing == nullptr) Database::AddAppointment(a); else Database::UpdateAppointment(a);
            Theme::Info(L"Appointment saved successfully.");
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
        }
        catch (Exception^ ex)
        {
            Theme::Error(ex->Message);
        }
    }
};
