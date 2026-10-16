#pragma once
#include "Theme.h"
#include "Database.h"
#include "BookingAppointmentForm.h"
#include "AppointmentForm.h"
#include "CustomerForm.h"
#include "StaffForm.h"
#include "ServiceForm.h"

// Main window: header, five horizontal tabs (Home / Services / Gallery / About Us / Contact Us)
// and the admin management buttons on the Home tab.
public ref class DashboardForm : public System::Windows::Forms::Form
{
private:
    String^ adminName;
    Panel^ contentPanel;
    array<Panel^>^ pages;
    array<Button^>^ navButtons;
    FlowLayoutPanel^ flowServices;
    Label^ lblStatCustomers;
    Label^ lblStatStaff;
    Label^ lblStatToday;
    Label^ lblStatUpcoming;
    TextBox^ txtMsgName;
    TextBox^ txtMsgEmail;
    TextBox^ txtMsgText;

public:
    property bool LogoutRequested;

    DashboardForm(String^ username)
    {
        adminName = username;
        LogoutRequested = false;
        Build();
        ShowPage(0);
    }

private:
    // ============================================================ layout
    void Build()
    {
        this->Text = L"Salon Zelora";
        this->ClientSize = Theme::Sz(1100, 660);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = false;
        this->BackColor = Theme::SkyLight();

        // Fill panel first, then the docked bars (the last one added ends up on top)
        contentPanel = gcnew Panel();
        contentPanel->Dock = DockStyle::Fill;
        contentPanel->BackColor = Theme::SkyLight();
        this->Controls->Add(contentPanel);
        this->Controls->Add(BuildNavBar());
        this->Controls->Add(BuildHeader());

        pages = gcnew array<Panel^>(5);
        pages[0] = BuildHomePage();
        pages[1] = BuildServicesPage();
        pages[2] = BuildGalleryPage();
        pages[3] = BuildAboutPage();
        pages[4] = BuildContactPage();
        for (int i = 0; i < 5; i++)
        {
            pages[i]->Dock = DockStyle::Fill;
            pages[i]->Visible = false;
            contentPanel->Controls->Add(pages[i]);
        }

        this->FormClosing += gcnew FormClosingEventHandler(this, &DashboardForm::OnClosing);
    }

    Panel^ BuildHeader()
    {
        Panel^ h = gcnew Panel();
        h->Dock = DockStyle::Top;
        h->Height = 80;
        h->BackColor = Theme::SkyBlue();

        h->Controls->Add(Theme::MakePicture(L"Logo.jpg", 20, 10, 60, 60));
        h->Controls->Add(Theme::MakeLabel(L"Salon Zelora", 95, 8, 24, true, Color::White));
        h->Controls->Add(Theme::MakeLabel(L"Management Portal", 99, 52, 10, false, Color::White));

        Label^ user = Theme::MakeParagraph(L"Logged in as: " + adminName, 700, 28, 250, 24, 10, false, Color::White, ContentAlignment::MiddleRight);
        h->Controls->Add(user);

        Button^ logout = Theme::MakeButton(L"Logout", 960, 22, 120, 36, Color::White, Theme::SkyDark());
        logout->Click += gcnew EventHandler(this, &DashboardForm::OnLogout);
        h->Controls->Add(logout);
        return h;
    }

    Panel^ BuildNavBar()
    {
        Panel^ n = gcnew Panel();
        n->Dock = DockStyle::Top;
        n->Height = 48;
        n->BackColor = Theme::SkyDark();

        TableLayoutPanel^ t = gcnew TableLayoutPanel();
        t->Dock = DockStyle::Fill;
        t->ColumnCount = 5;
        t->RowCount = 1;
        for (int i = 0; i < 5; i++) t->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 20.0f));
        t->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0f));

        array<String^>^ names = gcnew array<String^> { L"Home", L"Services", L"Gallery", L"About Us", L"Contact Us" };
        navButtons = gcnew array<Button^>(5);
        for (int i = 0; i < 5; i++)
        {
            Button^ b = gcnew Button();
            b->Text = names[i];
            b->Dock = DockStyle::Fill;
            b->Margin = System::Windows::Forms::Padding(0);
            b->FlatStyle = FlatStyle::Flat;
            b->FlatAppearance->BorderSize = 0;
            b->Font = Theme::F(11, true);
            b->Cursor = Cursors::Hand;
            b->Tag = i;
            b->Click += gcnew EventHandler(this, &DashboardForm::OnNav);
            navButtons[i] = b;
            t->Controls->Add(b, i, 0);
        }
        n->Controls->Add(t);
        return n;
    }

    Panel^ NewPage()
    {
        Panel^ p = gcnew Panel();
        p->BackColor = Theme::SkyLight();
        return p;
    }

    void ShowPage(int idx)
    {
        for (int i = 0; i < 5; i++)
        {
            bool active = (i == idx);
            pages[i]->Visible = active;
            if (active) { navButtons[i]->BackColor = Theme::SkyLight(); navButtons[i]->ForeColor = Theme::SkyDark(); }
            else { navButtons[i]->BackColor = Theme::SkyDark();  navButtons[i]->ForeColor = Color::White; }
        }
        if (idx == 0) RefreshStats();
        if (idx == 1) LoadServices();
    }

    // ============================================================ HOME
    Label^ AddStatCard(Panel^ page, int x, int y, String^ caption)
    {
        Panel^ card = gcnew Panel();
        card->Location = Theme::Pt(x, y);
        card->Size = Theme::Sz(250, 80);
        card->BackColor = Theme::Card();
        Label^ value = Theme::MakeLabel(L"0", 14, 6, 22, true, Theme::Navy());
        card->Controls->Add(value);
        card->Controls->Add(Theme::MakeLabel(caption, 16, 52, 10, false, Theme::Navy()));
        page->Controls->Add(card);
        return value;
    }

    Panel^ BuildHomePage()
    {
        Panel^ p = NewPage();
        Color navy = Theme::Navy();

        p->Controls->Add(Theme::MakePicture(L"Home.jpg", 30, 20, 570, 310));

        p->Controls->Add(Theme::MakeLabel(L"Welcome to Salon Zelora", 630, 28, 24, true, navy));
        p->Controls->Add(Theme::MakeParagraph(
            L"Your beauty, our passion. Enjoy professional hair, facial, makeup and nail care services "
            L"in a comfortable and welcoming environment. Book your appointment today and let us help you feel your best.",
            630, 85, 440, 120, 13, false, navy, ContentAlignment::TopLeft));

        Button^ book = Theme::MakeButton(L"BOOK AN APPOINTMENT", 630, 225, 320, 54, Theme::SkyBlue(), Color::White);
        book->Font = Theme::F(12, true);
        book->Click += gcnew EventHandler(this, &DashboardForm::OnBook);
        p->Controls->Add(book);

        lblStatCustomers = AddStatCard(p, 30, 350, L"Customers");
        lblStatStaff = AddStatCard(p, 295, 350, L"Staff members");
        lblStatToday = AddStatCard(p, 560, 350, L"Appointments today");
        lblStatUpcoming = AddStatCard(p, 825, 350, L"Upcoming appointments");

        p->Controls->Add(Theme::MakeLabel(L"Management", 30, 445, 13, true, navy));

        array<String^>^ names = gcnew array<String^> { L"Customer Management", L"Appointment Management", L"Service Management", L"Staff Management" };
        for (int i = 0; i < 4; i++)
        {
            Button^ b = Theme::MakeButton(names[i], 30 + i * 265, 480, 250, 50, Theme::SkyDark(), Color::White);
            b->Tag = i;
            b->Click += gcnew EventHandler(this, &DashboardForm::OnManage);
            p->Controls->Add(b);
        }
        return p;
    }

    void RefreshStats()
    {
        try
        {
            lblStatCustomers->Text = Convert::ToString(Database::Count(L"SELECT COUNT(*) FROM dbo.Customers"));
            lblStatStaff->Text = Convert::ToString(Database::Count(L"SELECT COUNT(*) FROM dbo.Staff"));
            lblStatToday->Text = Convert::ToString(Database::Count(
                L"SELECT COUNT(*) FROM dbo.Appointments WHERE CAST(AppointmentTime AS DATE) = CAST(GETDATE() AS DATE) AND Status <> N'Cancelled'"));
            lblStatUpcoming->Text = Convert::ToString(Database::Count(
                L"SELECT COUNT(*) FROM dbo.Appointments WHERE AppointmentTime >= GETDATE() AND Status IN (N'Pending', N'Confirmed')"));
        }
        catch (Exception^) {}
    }

    // ============================================================ SERVICES
    Panel^ BuildServicesPage()
    {
        Panel^ p = NewPage();
        p->Controls->Add(Theme::MakeParagraph(L"OUR SERVICES", 0, 8, 1100, 56, 26, true, Theme::Navy(), ContentAlignment::MiddleCenter));

        flowServices = gcnew FlowLayoutPanel();
        flowServices->Location = Theme::Pt(40, 70);
        flowServices->Size = Theme::Sz(1020, 370);
        flowServices->AutoScroll = true;
        flowServices->WrapContents = true;
        flowServices->FlowDirection = FlowDirection::LeftToRight;
        flowServices->BackColor = Theme::SkyLight();
        p->Controls->Add(flowServices);

        Button^ book = Theme::MakeButton(L"BOOK APPOINTMENT", 390, 455, 320, 52, Theme::SkyBlue(), Color::White);
        book->Font = Theme::F(12, true);
        book->Click += gcnew EventHandler(this, &DashboardForm::OnBook);
        p->Controls->Add(book);
        return p;
    }

    void LoadServices()
    {
        flowServices->Controls->Clear();
        try
        {
            DataTable^ dt = Database::GetServices();
            for each (DataRow ^ r in dt->Rows)
            {
                Panel^ card = gcnew Panel();
                card->Size = Theme::Sz(470, 100);
                card->BackColor = Theme::Card();
                card->Margin = System::Windows::Forms::Padding(10);

                card->Controls->Add(Theme::MakeLabel(Convert::ToString(r[L"Name"]), 15, 8, 16, true, Theme::Navy()));
                card->Controls->Add(Theme::MakeLabel(Convert::ToString(r[L"Description"]), 17, 42, 11, false, Theme::Navy()));

                Decimal price = Convert::ToDecimal(r[L"Price"]);
                String^ info = L"LKR " + price.ToString(L"N2") + L"   |   " + Convert::ToString(r[L"DurationMinutes"]) + L" min";
                card->Controls->Add(Theme::MakeLabel(info, 17, 70, 10, true, Theme::SkyDark()));

                flowServices->Controls->Add(card);
            }
        }
        catch (Exception^ ex) { Theme::Error(L"Could not load services.\n\n" + ex->Message); }
    }

    // ============================================================ GALLERY
    Panel^ BuildGalleryPage()
    {
        Panel^ p = NewPage();
        p->Controls->Add(Theme::MakeParagraph(L"OUR GALLERY", 0, 4, 1100, 54, 26, true, Theme::Navy(), ContentAlignment::MiddleCenter));

        array<String^>^ files = gcnew array<String^> { L"Gallery1.jpg", L"Gallery2.jpg", L"Gallery3.jpg", L"Gallery4.jpg", L"Gallery5.jpg" };
        array<int>^ xs = gcnew array<int> { 30, 550, 30, 385, 740 };
        array<int>^ ys = gcnew array<int> { 62, 62, 290, 290, 290 };
        array<int>^ ws = gcnew array<int> { 520, 520, 330, 330, 330 };
        for (int i = 0; i < 5; i++)
        {
            PictureBox^ pic = Theme::MakePicture(files[i], xs[i], ys[i], ws[i], 218);
            pic->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            p->Controls->Add(pic);
        }
        return p;
    }

    // ============================================================ ABOUT US
    Panel^ BuildAboutPage()
    {
        Panel^ p = NewPage();
        Color navy = Theme::Navy();
        ContentAlignment mc = ContentAlignment::MiddleCenter;

        p->Controls->Add(Theme::MakeParagraph(L"ABOUT SALON ZELORA", 0, 10, 1100, 60, 28, true, navy, mc));
        p->Controls->Add(Theme::MakeParagraph(
            L"At Salon ZELORA, we believe that beauty is about confidence, care and feeling your best.",
            100, 80, 900, 55, 15, false, navy, mc));
        p->Controls->Add(Theme::MakeParagraph(
            L"Our professional team is dedicated to providing quality beauty services in a comfortable and welcoming environment.",
            100, 140, 900, 55, 15, false, navy, mc));
        p->Controls->Add(Theme::MakeParagraph(L"Your beauty, our passion.", 100, 200, 900, 36, 15, true, navy, mc));

        p->Controls->Add(Theme::MakeParagraph(L"OUR MISSION", 100, 250, 900, 34, 16, true, navy, mc));
        p->Controls->Add(Theme::MakeParagraph(
            L"To provide high-quality beauty services with care, professionalism and creativity.",
            100, 285, 900, 34, 14, false, navy, mc));

        p->Controls->Add(Theme::MakeParagraph(L"WHY CHOOSE US?", 100, 335, 900, 34, 16, true, navy, mc));
        array<String^>^ pts = gcnew array<String^> {
            L"\u2713  Professional service", L"\u2713  Friendly environment",
                L"\u2713  Quality beauty care", L"\u2713  Customer satisfaction" };
        for (int i = 0; i < 4; i++)
            p->Controls->Add(Theme::MakeParagraph(pts[i], 100, 375 + i * 34, 900, 32, 14, false, navy, mc));
        return p;
    }

    // ============================================================ CONTACT US
    Panel^ BuildContactPage()
    {
        Panel^ p = NewPage();
        Color navy = Theme::Navy();

        p->Controls->Add(Theme::MakeParagraph(L"CONTACT US", 0, 8, 1100, 56, 26, true, navy, ContentAlignment::MiddleCenter));

        Panel^ info = gcnew Panel();
        info->Location = Theme::Pt(50, 100);
        info->Size = Theme::Sz(470, 230);
        info->BackColor = Theme::Card();
        info->Controls->Add(Theme::MakeLabel(L"Address", 25, 30, 14, true, navy));
        info->Controls->Add(Theme::MakeLabel(L"Mihinthale, Sri Lanka", 160, 32, 13, false, navy));
        info->Controls->Add(Theme::MakeLabel(L"Phone", 25, 100, 14, true, navy));
        info->Controls->Add(Theme::MakeLabel(L"042 11 222 333", 160, 102, 13, false, navy));
        info->Controls->Add(Theme::MakeLabel(L"Email", 25, 170, 14, true, navy));
        info->Controls->Add(Theme::MakeLabel(L"info@salonzelora.com", 160, 172, 13, false, navy));
        p->Controls->Add(info);

        p->Controls->Add(Theme::MakeLabel(L"Send Us a Message", 570, 80, 16, true, navy));
        p->Controls->Add(Theme::MakeLabel(L"Your Name", 570, 129, 10, true, navy));
        txtMsgName = Theme::MakeTextBox(690, 125, 360); p->Controls->Add(txtMsgName);
        p->Controls->Add(Theme::MakeLabel(L"Your Email", 570, 174, 10, true, navy));
        txtMsgEmail = Theme::MakeTextBox(690, 170, 360); p->Controls->Add(txtMsgEmail);
        p->Controls->Add(Theme::MakeLabel(L"Message", 570, 219, 10, true, navy));
        txtMsgText = Theme::MakeTextBox(690, 215, 360);
        txtMsgText->Multiline = true;
        txtMsgText->Height = 170;
        txtMsgText->MaxLength = 1000;
        p->Controls->Add(txtMsgText);

        Button^ send = Theme::MakeButton(L"SEND MESSAGE", 690, 400, 220, 46, Theme::SkyBlue(), Color::White);
        send->Click += gcnew EventHandler(this, &DashboardForm::OnSendMessage);
        p->Controls->Add(send);
        return p;
    }

    // ============================================================ events
    void OnNav(Object^ sender, EventArgs^ e)
    {
        Button^ b = safe_cast<Button^>(sender);
        ShowPage(Convert::ToInt32(b->Tag));
    }

    void OnBook(Object^ sender, EventArgs^ e)
    {
        BookingAppointmentForm^ f = gcnew BookingAppointmentForm(nullptr);
        f->ShowDialog(this);
        RefreshStats();
    }

    void OnManage(Object^ sender, EventArgs^ e)
    {
        Button^ b = safe_cast<Button^>(sender);
        int which = Convert::ToInt32(b->Tag);
        if (which == 0) { CustomerForm^ f = gcnew CustomerForm();       f->ShowDialog(this); }
        else if (which == 1) { AppointmentForm^ f = gcnew AppointmentForm(); f->ShowDialog(this); }
        else if (which == 2) { ServiceForm^ f = gcnew ServiceForm();         f->ShowDialog(this); }
        else { StaffForm^ f = gcnew StaffForm();             f->ShowDialog(this); }
        RefreshStats();
    }

    void OnSendMessage(Object^ sender, EventArgs^ e)
    {
        String^ n = txtMsgName->Text->Trim();
        String^ em = txtMsgEmail->Text->Trim();
        String^ m = txtMsgText->Text->Trim();
        if (n->Length == 0) { Theme::Warn(L"Please enter your name."); return; }
        if (!Theme::IsValidEmail(em)) { Theme::Warn(L"Please enter a valid email address."); return; }
        if (m->Length == 0) { Theme::Warn(L"Please type your message."); return; }
        try
        {
            Database::SaveMessage(n, em, m);
            Theme::Info(L"Thank you! Your message has been sent.");
            txtMsgName->Clear(); txtMsgEmail->Clear(); txtMsgText->Clear();
        }
        catch (Exception^ ex) { Theme::Error(ex->Message); }
    }

    void OnLogout(Object^ sender, EventArgs^ e)
    {
        if (Theme::Confirm(L"Do you want to log out?"))
        {
            LogoutRequested = true;
            this->Close();
        }
    }

    void OnClosing(Object^ sender, FormClosingEventArgs^ e)
    {
        if (!LogoutRequested && e->CloseReason == CloseReason::UserClosing)
        {
            if (!Theme::Confirm(L"Exit Salon Zelora?")) e->Cancel = true;
        }
    }
};
