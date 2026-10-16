#pragma once

#using <System.dll>
#using <System.Data.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <System.Xml.dll>

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Text::RegularExpressions;

// Shared colours, fonts, control factories and validation helpers (sky-blue theme)
public ref class Theme abstract sealed
{
public:
    // ---------- colours ----------
    static Color SkyBlue() { return Color::FromArgb(0, 150, 214); }
    static Color SkyDark() { return Color::FromArgb(0, 112, 170); }
    static Color SkyLight() { return Color::FromArgb(232, 246, 253); }
    static Color Card() { return Color::FromArgb(163, 218, 236); }
    static Color Navy() { return Color::FromArgb(10, 36, 99); }
    static Color Danger() { return Color::FromArgb(220, 53, 69); }
    static Color Grey() { return Color::FromArgb(210, 210, 210); }

    // ---------- small helpers ----------
    static System::Drawing::Point Pt(int x, int y) { return System::Drawing::Point(x, y); }
    static System::Drawing::Size  Sz(int w, int h) { return System::Drawing::Size(w, h); }

    static System::Drawing::Font^ F(float size, bool bold)
    {
        return gcnew System::Drawing::Font(L"Segoe UI", size, bold ? FontStyle::Bold : FontStyle::Regular);
    }

    static AnchorStyles AnchorAll()
    {
        return static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Bottom | AnchorStyles::Left | AnchorStyles::Right);
    }
    static AnchorStyles AnchorTopRight()
    {
        return static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Right);
    }

    // ---------- images ----------
    // Looks for the file in the working directory, the exe folder and up to 6 parent folders.
    static Image^ LoadImage(String^ fileName)
    {
        array<String^>^ roots = gcnew array<String^> { Environment::CurrentDirectory, AppDomain::CurrentDomain->BaseDirectory };
        for each (String ^ root in roots)
        {
            String^ dir = root;
            for (int i = 0; i < 7 && !String::IsNullOrEmpty(dir); i++)
            {
                String^ path = Path::Combine(dir, fileName);
                if (File::Exists(path))
                {
                    try
                    {
                        array<Byte>^ bytes = File::ReadAllBytes(path);   // no file lock
                        MemoryStream^ ms = gcnew MemoryStream(bytes);
                        return Image::FromStream(ms);
                    }
                    catch (Exception^) { return nullptr; }
                }
                DirectoryInfo^ parent = Directory::GetParent(dir);
                if (parent == nullptr) dir = nullptr; else dir = parent->FullName;
            }
        }
        return nullptr;
    }

    static PictureBox^ MakePicture(String^ file, int x, int y, int w, int h)
    {
        PictureBox^ p = gcnew PictureBox();
        p->Location = Pt(x, y);
        p->Size = Sz(w, h);
        p->SizeMode = PictureBoxSizeMode::Zoom;
        p->BackColor = SkyLight();
        Image^ img = LoadImage(file);
        if (img != nullptr) p->Image = img; else p->BackColor = Card();
        return p;
    }

    // ---------- control factories ----------
    static Label^ MakeLabel(String^ text, int x, int y, float size, bool bold, Color color)
    {
        Label^ l = gcnew Label();
        l->Text = text;
        l->Location = Pt(x, y);
        l->AutoSize = true;
        l->Font = F(size, bold);
        l->ForeColor = color;
        l->BackColor = Color::Transparent;
        return l;
    }

    static Label^ MakeParagraph(String^ text, int x, int y, int w, int h, float size, bool bold, Color color, ContentAlignment align)
    {
        Label^ l = gcnew Label();
        l->Text = text;
        l->Location = Pt(x, y);
        l->AutoSize = false;
        l->Size = Sz(w, h);
        l->Font = F(size, bold);
        l->ForeColor = color;
        l->BackColor = Color::Transparent;
        l->TextAlign = align;
        return l;
    }

    static TextBox^ MakeTextBox(int x, int y, int w)
    {
        TextBox^ t = gcnew TextBox();
        t->Location = Pt(x, y);
        t->Width = w;
        t->Font = F(10, false);
        t->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
        return t;
    }

    static ComboBox^ MakeCombo(int x, int y, int w)
    {
        ComboBox^ c = gcnew ComboBox();
        c->Location = Pt(x, y);
        c->Width = w;
        c->Font = F(10, false);
        c->DropDownStyle = ComboBoxStyle::DropDownList;
        return c;
    }

    static Button^ MakeButton(String^ text, int x, int y, int w, int h, Color back, Color fore)
    {
        Button^ b = gcnew Button();
        b->Text = text;
        b->Location = Pt(x, y);
        b->Size = Sz(w, h);
        b->FlatStyle = FlatStyle::Flat;
        b->FlatAppearance->BorderSize = 0;
        b->BackColor = back;
        b->ForeColor = fore;
        b->Font = F(10, true);
        b->Cursor = Cursors::Hand;
        return b;
    }

    static Panel^ MakeHeaderBar(String^ title)
    {
        Panel^ p = gcnew Panel();
        p->Dock = DockStyle::Top;
        p->Height = 70;
        p->BackColor = SkyBlue();
        Label^ l = gcnew Label();
        l->Dock = DockStyle::Fill;
        l->Text = title;
        l->TextAlign = ContentAlignment::MiddleCenter;
        l->ForeColor = Color::White;
        l->Font = F(20, true);
        p->Controls->Add(l);
        return p;
    }

    static void StyleGrid(DataGridView^ g)
    {
        g->ReadOnly = true;
        g->AllowUserToAddRows = false;
        g->AllowUserToDeleteRows = false;
        g->AllowUserToResizeRows = false;
        g->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        g->MultiSelect = false;
        g->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
        g->BackgroundColor = Color::White;
        g->BorderStyle = System::Windows::Forms::BorderStyle::None;
        g->RowHeadersVisible = false;
        g->EnableHeadersVisualStyles = false;
        g->ColumnHeadersDefaultCellStyle->BackColor = SkyBlue();
        g->ColumnHeadersDefaultCellStyle->ForeColor = Color::White;
        g->ColumnHeadersDefaultCellStyle->Font = F(10, true);
        g->ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode::DisableResizing;
        g->ColumnHeadersHeight = 34;
        g->AlternatingRowsDefaultCellStyle->BackColor = SkyLight();
        g->DefaultCellStyle->Font = F(10, false);
        g->DefaultCellStyle->SelectionBackColor = SkyDark();
        g->DefaultCellStyle->SelectionForeColor = Color::White;
        g->RowTemplate->Height = 28;
    }

    // ---------- validation ----------
    static bool IsValidEmail(String^ s) { return Regex::IsMatch(s, L"^[^@\\s]+@[^@\\s]+\\.[^@\\s]+$"); }
    static bool IsValidPhone(String^ s) { return Regex::IsMatch(s, L"^\\+?[0-9 \\-]{9,15}$"); }

    // ---------- message boxes ----------
    static void Info(String^ m) { MessageBox::Show(m, L"Salon Zelora", MessageBoxButtons::OK, MessageBoxIcon::Information); }
    static void Warn(String^ m) { MessageBox::Show(m, L"Salon Zelora", MessageBoxButtons::OK, MessageBoxIcon::Warning); }
    static void Error(String^ m) { MessageBox::Show(m, L"Salon Zelora", MessageBoxButtons::OK, MessageBoxIcon::Error); }
    static bool Confirm(String^ m)
    {
        return MessageBox::Show(m, L"Salon Zelora", MessageBoxButtons::YesNo, MessageBoxIcon::Question)
            == System::Windows::Forms::DialogResult::Yes;
    }
};
