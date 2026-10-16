#include "Theme.h"
#include "Database.h"
#include "LoginForm.h"
#include "DashboardForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    try
    {
        Database::Initialize();   // creates ZeloraDB, tables and default data on first run
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(L"Could not connect to SQL Server LocalDB.\n\n" + ex->Message,
            L"Salon Zelora", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return 1;
    }

    bool again = true;
    while (again)                 // logging out returns to the login screen
    {
        again = false;
        LoginForm^ login = gcnew LoginForm();
        if (login->ShowDialog() == System::Windows::Forms::DialogResult::OK)
        {
            DashboardForm^ dash = gcnew DashboardForm(login->Username);
            dash->ShowDialog();
            again = dash->LogoutRequested;
        }
    }
    return 0;
}
