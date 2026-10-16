#pragma once
#include "Theme.h"
#include "Database.h"

public ref class LoginForm : public System::Windows::Forms::Form
{
private:
    TextBox^ txtUser;
    TextBox^ txtPass;
    Label^   lblError;

public:
    property String^ Username;

    LoginForm()
    {
        Username = L"";
        this->Text = L"Salon Zelora - Login";
        this->ClientSize = Theme::Sz(560, 470);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = false;
        this->BackColor = Color::White;

        Panel^ header = gcnew Panel();
        header->Dock = DockStyle::Top;
        header->Height = 80;
        header->BackColor = Theme::SkyBlue();
        Label^ title = gcnew Label();
        title->Dock = DockStyle::Fill;
        title->Text = L"Salon Zelora - Admin Login";
        title->TextAlign = ContentAlignment::MiddleCenter;
        title->ForeColor = Color::White;
        title->Font = Theme::F(20, true);
        header->Controls->Add(title);
        this->Controls->Add(header);

        this->Controls->Add(Theme::MakePicture(L"Logo.jpg", 225, 100, 110, 110));

        this->Controls->Add(Theme::MakeLabel(L"Username:", 100, 254, 11, true, Color::Black));
        txtUser = Theme::MakeTextBox(210, 250, 250);
        this->Controls->Add(txtUser);

        this->Controls->Add(Theme::MakeLabel(L"Password:", 100, 302, 11, true, Color::Black));
        txtPass = Theme::MakeTextBox(210, 298, 250);
        txtPass->UseSystemPasswordChar = true;
        this->Controls->Add(txtPass);

        lblError = Theme::MakeLabel(L"", 210, 336, 9, false, Theme::Danger());
        this->Controls->Add(lblError);

        Button^ btnLogin = Theme::MakeButton(L"Login", 150, 380, 120, 44, Theme::SkyBlue(), Color::White);
        btnLogin->Click += gcnew EventHandler(this, &LoginForm::OnLogin);
        this->Controls->Add(btnLogin);

        Button^ btnExit = Theme::MakeButton(L"Exit", 290, 380, 120, 44, Theme::Grey(), Color::Black);
        btnExit->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->Controls->Add(btnExit);

        this->AcceptButton = btnLogin;
    }

private:
    void OnLogin(Object^ sender, EventArgs^ e)
    {
        String^ u = txtUser->Text->Trim();
        String^ p = txtPass->Text;
        if (u->Length == 0 || p->Length == 0)
        {
            lblError->Text = L"Please enter your username and password.";
            return;
        }
        try
        {
            if (Database::ValidateLogin(u, p))
            {
                Username = u;
                this->DialogResult = System::Windows::Forms::DialogResult::OK;
            }
            else
            {
                lblError->Text = L"Invalid username or password.";
                txtPass->Clear();
                txtPass->Focus();
            }
        }
        catch (Exception^ ex)
        {
            Theme::Error(L"Could not connect to the database.\n\n" + ex->Message);
        }
    }
};
