#pragma once
#include "Theme.h"
#include "Customer.h"
#include "Staff.h"
#include "Service.h"
#include "Appointment.h"
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Security::Cryptography;
using namespace System::Text;

// All database access for Salon Zelora (SQL Server Express, database "ZeloraDB").
// Updated to use the installed instance: localhost\SQLEXPRESS01
public ref class Database abstract sealed
{
private:
    static String^ ConnStr(String^ catalog)
    {
        return L"Data Source=localhost\\SQLEXPRESS01;Initial Catalog=" + catalog +
            L";Integrated Security=True;Connect Timeout=30;";
    }

public:
    // ------------------------------------------------------------ helpers
    static String^ Hash(String^ text)
    {
        SHA256^ sha = SHA256::Create();
        array<Byte>^ b = sha->ComputeHash(Encoding::UTF8->GetBytes(text));
        StringBuilder^ sb = gcnew StringBuilder();
        for each (Byte x in b) sb->Append(x.ToString(L"x2"));
        return sb->ToString();
    }

    static SqlParameter^ P(String^ name, Object^ value)
    {
        if (value == nullptr) return gcnew SqlParameter(name, DBNull::Value);
        return gcnew SqlParameter(name, value);
    }

    static DataTable^ Query(String^ sql, array<SqlParameter^>^ ps)
    {
        SqlConnection^ con = gcnew SqlConnection(ConnStr(L"ZeloraDB"));
        try
        {
            SqlCommand^ cmd = gcnew SqlCommand(sql, con);
            if (ps != nullptr) cmd->Parameters->AddRange(ps);
            SqlDataAdapter^ ad = gcnew SqlDataAdapter(cmd);
            DataTable^ dt = gcnew DataTable();
            ad->Fill(dt);
            return dt;
        }
        finally { delete con; }
    }

    static int Exec(String^ sql, array<SqlParameter^>^ ps)
    {
        SqlConnection^ con = gcnew SqlConnection(ConnStr(L"ZeloraDB"));
        try
        {
            con->Open();
            SqlCommand^ cmd = gcnew SqlCommand(sql, con);
            if (ps != nullptr) cmd->Parameters->AddRange(ps);
            return cmd->ExecuteNonQuery();
        }
        finally { delete con; }
    }

    static int Count(String^ sql, array<SqlParameter^>^ ps)
    {
        SqlConnection^ con = gcnew SqlConnection(ConnStr(L"ZeloraDB"));
        try
        {
            con->Open();
            SqlCommand^ cmd = gcnew SqlCommand(sql, con);
            if (ps != nullptr) cmd->Parameters->AddRange(ps);
            return Convert::ToInt32(cmd->ExecuteScalar());
        }
        finally { delete con; }
    }

    static int Count(String^ sql) { return Count(sql, nullptr); }

    static bool Exists(String^ table, String^ column, String^ value)
    {
        return Count(L"SELECT COUNT(*) FROM dbo." + table + L" WHERE " + column + L"=@v",
            gcnew array<SqlParameter^> { P(L"@v", value) }) > 0;
    }

    // Suggests the next ID, e.g. prefix "C" -> C001, C002 ...
    static String^ NextId(String^ table, String^ column, String^ prefix)
    {
        DataTable^ dt = Query(L"SELECT " + column + L" FROM dbo." + table, nullptr);
        int max = 0;
        for each (DataRow ^ r in dt->Rows)
        {
            String^ s = Convert::ToString(r[0]);
            if (s->StartsWith(prefix))
            {
                int n = 0;
                if (Int32::TryParse(s->Substring(prefix->Length), n) && n > max) max = n;
            }
        }
        int next = max + 1;
        return prefix + next.ToString(L"000");
    }

    // ------------------------------------------------------------ setup
    static void Initialize()
    {
        SqlConnection^ c = gcnew SqlConnection(ConnStr(L"master"));
        try
        {
            c->Open();
            SqlCommand^ cmd = gcnew SqlCommand(L"IF DB_ID(N'ZeloraDB') IS NULL CREATE DATABASE ZeloraDB", c);
            cmd->ExecuteNonQuery();
        }
        finally { delete c; }

        Exec(L"IF OBJECT_ID(N'dbo.Users', N'U') IS NULL CREATE TABLE dbo.Users ("
            L"UserId INT IDENTITY(1,1) PRIMARY KEY, "
            L"Username NVARCHAR(50) NOT NULL UNIQUE, "
            L"PasswordHash NVARCHAR(64) NOT NULL)", nullptr);

        Exec(L"IF OBJECT_ID(N'dbo.Customers', N'U') IS NULL CREATE TABLE dbo.Customers ("
            L"CustomerId NVARCHAR(20) PRIMARY KEY, "
            L"Name NVARCHAR(100) NOT NULL, "
            L"Phone NVARCHAR(20) NOT NULL, "
            L"Email NVARCHAR(100) NOT NULL DEFAULT N'')", nullptr);

        Exec(L"IF OBJECT_ID(N'dbo.Staff', N'U') IS NULL CREATE TABLE dbo.Staff ("
            L"StaffId NVARCHAR(20) PRIMARY KEY, "
            L"Name NVARCHAR(100) NOT NULL, "
            L"Position NVARCHAR(50) NOT NULL, "
            L"Phone NVARCHAR(20) NOT NULL, "
            L"Email NVARCHAR(100) NOT NULL DEFAULT N'')", nullptr);

        Exec(L"IF OBJECT_ID(N'dbo.Services', N'U') IS NULL CREATE TABLE dbo.Services ("
            L"ServiceId INT IDENTITY(1,1) PRIMARY KEY, "
            L"Name NVARCHAR(100) NOT NULL, "
            L"Description NVARCHAR(250) NOT NULL DEFAULT N'', "
            L"Price DECIMAL(10,2) NOT NULL DEFAULT 0, "
            L"DurationMinutes INT NOT NULL DEFAULT 60)", nullptr);

        Exec(L"IF OBJECT_ID(N'dbo.Appointments', N'U') IS NULL CREATE TABLE dbo.Appointments ("
            L"AppointmentId INT IDENTITY(1,1) PRIMARY KEY, "
            L"CustomerName NVARCHAR(100) NOT NULL, "
            L"CustomerEmail NVARCHAR(100) NOT NULL DEFAULT N'', "
            L"Phone NVARCHAR(20) NOT NULL, "
            L"ServiceId INT NULL REFERENCES dbo.Services(ServiceId) ON DELETE SET NULL, "
            L"StaffId NVARCHAR(20) NULL REFERENCES dbo.Staff(StaffId) ON DELETE SET NULL, "
            L"AppointmentTime DATETIME NOT NULL, "
            L"Status NVARCHAR(20) NOT NULL DEFAULT N'Pending', "
            L"Notes NVARCHAR(500) NOT NULL DEFAULT N'', "
            L"CreatedAt DATETIME NOT NULL DEFAULT GETDATE())", nullptr);

        Exec(L"IF OBJECT_ID(N'dbo.Messages', N'U') IS NULL CREATE TABLE dbo.Messages ("
            L"MessageId INT IDENTITY(1,1) PRIMARY KEY, "
            L"SenderName NVARCHAR(100) NOT NULL, "
            L"SenderEmail NVARCHAR(100) NOT NULL, "
            L"MessageText NVARCHAR(1000) NOT NULL, "
            L"SentAt DATETIME NOT NULL DEFAULT GETDATE())", nullptr);

        // default admin account
        Exec(L"IF NOT EXISTS (SELECT 1 FROM dbo.Users WHERE Username=@u) "
            L"INSERT INTO dbo.Users (Username, PasswordHash) VALUES (@u, @p)",
            gcnew array<SqlParameter^> { P(L"@u", L"admin"), P(L"@p", Hash(L"admin123")) });

        // default services (editable from Service Management)
        Exec(L"IF NOT EXISTS (SELECT 1 FROM dbo.Services) "
            L"INSERT INTO dbo.Services (Name, Description, Price, DurationMinutes) VALUES "
            L"(N'Hair Styling', N'Hair Cut & Styling', 2500, 60), "
            L"(N'Facial', N'Relaxing Facial Treatments', 3500, 60), "
            L"(N'Makeup', N'Professional Makeup', 6000, 90), "
            L"(N'Nail Care', N'Manicure & Pedicure', 3000, 75)", nullptr);
    }

    // ------------------------------------------------------------ login
    static bool ValidateLogin(String^ user, String^ pass)
    {
        return Count(L"SELECT COUNT(*) FROM dbo.Users WHERE Username=@u AND PasswordHash=@p",
            gcnew array<SqlParameter^> { P(L"@u", user), P(L"@p", Hash(pass)) }) > 0;
    }

    // ------------------------------------------------------------ customers
    static DataTable^ GetCustomers(String^ search)
    {
        return Query(L"SELECT CustomerId AS [Customer ID], Name, Phone, Email FROM dbo.Customers "
            L"WHERE CustomerId LIKE @s OR Name LIKE @s OR Phone LIKE @s OR Email LIKE @s ORDER BY CustomerId",
            gcnew array<SqlParameter^> { P(L"@s", L"%" + search + L"%") });
    }

    static void AddCustomer(Customer^ c)
    {
        Exec(L"INSERT INTO dbo.Customers (CustomerId, Name, Phone, Email) VALUES (@id, @n, @p, @e)",
            gcnew array<SqlParameter^> { P(L"@id", c->Id), P(L"@n", c->Name), P(L"@p", c->Phone), P(L"@e", c->Email) });
    }

    static void UpdateCustomer(Customer^ c)
    {
        Exec(L"UPDATE dbo.Customers SET Name=@n, Phone=@p, Email=@e WHERE CustomerId=@id",
            gcnew array<SqlParameter^> { P(L"@id", c->Id), P(L"@n", c->Name), P(L"@p", c->Phone), P(L"@e", c->Email) });
    }

    static void DeleteCustomer(String^ id)
    {
        Exec(L"DELETE FROM dbo.Customers WHERE CustomerId=@id", gcnew array<SqlParameter^> { P(L"@id", id) });
    }

    // ------------------------------------------------------------ staff
    static DataTable^ GetStaff(String^ search)
    {
        return Query(L"SELECT StaffId AS [Staff ID], Name, Position, Phone, Email FROM dbo.Staff "
            L"WHERE StaffId LIKE @s OR Name LIKE @s OR Position LIKE @s OR Phone LIKE @s OR Email LIKE @s ORDER BY StaffId",
            gcnew array<SqlParameter^> { P(L"@s", L"%" + search + L"%") });
    }

    static DataTable^ GetStaffList()
    {
        return Query(L"SELECT StaffId, Name + N' (' + Position + N')' AS Name FROM dbo.Staff ORDER BY StaffId", nullptr);
    }

    static void AddStaff(Staff^ s)
    {
        Exec(L"INSERT INTO dbo.Staff (StaffId, Name, Position, Phone, Email) VALUES (@id, @n, @pos, @p, @e)",
            gcnew array<SqlParameter^> { P(L"@id", s->Id), P(L"@n", s->Name), P(L"@pos", s->Position), P(L"@p", s->Phone), P(L"@e", s->Email) });
    }

    static void UpdateStaff(Staff^ s)
    {
        Exec(L"UPDATE dbo.Staff SET Name=@n, Position=@pos, Phone=@p, Email=@e WHERE StaffId=@id",
            gcnew array<SqlParameter^> { P(L"@id", s->Id), P(L"@n", s->Name), P(L"@pos", s->Position), P(L"@p", s->Phone), P(L"@e", s->Email) });
    }

    static void DeleteStaff(String^ id)
    {
        Exec(L"DELETE FROM dbo.Staff WHERE StaffId=@id", gcnew array<SqlParameter^> { P(L"@id", id) });
    }

    // ------------------------------------------------------------ services
    static DataTable^ GetServices()
    {
        return Query(L"SELECT ServiceId, Name, Description, Price, DurationMinutes FROM dbo.Services ORDER BY ServiceId", nullptr);
    }

    static void AddService(Service^ s)
    {
        Exec(L"INSERT INTO dbo.Services (Name, Description, Price, DurationMinutes) VALUES (@n, @d, @pr, @du)",
            gcnew array<SqlParameter^> { P(L"@n", s->Name), P(L"@d", s->Description), P(L"@pr", s->Price), P(L"@du", s->DurationMinutes) });
    }

    static void UpdateService(Service^ s)
    {
        Exec(L"UPDATE dbo.Services SET Name=@n, Description=@d, Price=@pr, DurationMinutes=@du WHERE ServiceId=@id",
            gcnew array<SqlParameter^> { P(L"@id", s->Id), P(L"@n", s->Name), P(L"@d", s->Description), P(L"@pr", s->Price), P(L"@du", s->DurationMinutes) });
    }

    static void DeleteService(int id)
    {
        Exec(L"DELETE FROM dbo.Services WHERE ServiceId=@id", gcnew array<SqlParameter^> { P(L"@id", id) });
    }

    // ------------------------------------------------------------ appointments
    static DataTable^ GetAppointments(String^ search, String^ status)
    {
        return Query(
            L"SELECT a.AppointmentId, a.CustomerName, a.CustomerEmail, a.Phone, "
            L"ISNULL(s.Name, N'-') AS Service, ISNULL(st.Name, N'-') AS Staff, "
            L"a.AppointmentTime, a.Status, a.Notes "
            L"FROM dbo.Appointments a "
            L"LEFT JOIN dbo.Services s ON a.ServiceId = s.ServiceId "
            L"LEFT JOIN dbo.Staff st ON a.StaffId = st.StaffId "
            L"WHERE (a.CustomerName LIKE @s OR a.Phone LIKE @s OR a.CustomerEmail LIKE @s) "
            L"AND (@st = N'All' OR a.Status = @st) "
            L"ORDER BY a.AppointmentTime DESC",
            gcnew array<SqlParameter^> { P(L"@s", L"%" + search + L"%"), P(L"@st", status) });
    }

    static Appointment^ GetAppointment(int id)
    {
        DataTable^ dt = Query(L"SELECT * FROM dbo.Appointments WHERE AppointmentId=@id",
            gcnew array<SqlParameter^> { P(L"@id", id) });
        if (dt->Rows->Count == 0) return nullptr;
        DataRow^ r = dt->Rows[0];
        Appointment^ a = gcnew Appointment();
        a->Id = Convert::ToInt32(r[L"AppointmentId"]);
        a->CustomerName = Convert::ToString(r[L"CustomerName"]);
        a->CustomerEmail = Convert::ToString(r[L"CustomerEmail"]);
        a->Phone = Convert::ToString(r[L"Phone"]);
        if (!r->IsNull(L"ServiceId")) a->ServiceId = Convert::ToInt32(r[L"ServiceId"]);
        if (!r->IsNull(L"StaffId")) a->StaffId = Convert::ToString(r[L"StaffId"]);
        a->AppointmentTime = Convert::ToDateTime(r[L"AppointmentTime"]);
        a->Status = Convert::ToString(r[L"Status"]);
        a->Notes = Convert::ToString(r[L"Notes"]);
        return a;
    }

    static void AddAppointment(Appointment^ a)
    {
        CheckConflict(a);
        Exec(L"INSERT INTO dbo.Appointments (CustomerName, CustomerEmail, Phone, ServiceId, StaffId, AppointmentTime, Status, Notes) "
            L"VALUES (@n, @e, @p, @sv, @st, @t, @status, @notes)", ApptParams(a));
        EnsureCustomer(a->CustomerName, a->Phone, a->CustomerEmail);
    }

    static void UpdateAppointment(Appointment^ a)
    {
        CheckConflict(a);
        Exec(L"UPDATE dbo.Appointments SET CustomerName=@n, CustomerEmail=@e, Phone=@p, ServiceId=@sv, StaffId=@st, "
            L"AppointmentTime=@t, Status=@status, Notes=@notes WHERE AppointmentId=@id", ApptParams(a));
    }

    static void UpdateAppointmentStatus(int id, String^ status)
    {
        Exec(L"UPDATE dbo.Appointments SET Status=@s WHERE AppointmentId=@id",
            gcnew array<SqlParameter^> { P(L"@s", status), P(L"@id", id) });
    }

    static void DeleteAppointment(int id)
    {
        Exec(L"DELETE FROM dbo.Appointments WHERE AppointmentId=@id", gcnew array<SqlParameter^> { P(L"@id", id) });
    }

    // ------------------------------------------------------------ messages
    static void SaveMessage(String^ name, String^ email, String^ text)
    {
        Exec(L"INSERT INTO dbo.Messages (SenderName, SenderEmail, MessageText) VALUES (@n, @e, @m)",
            gcnew array<SqlParameter^> { P(L"@n", name), P(L"@e", email), P(L"@m", text) });
    }

private:
    static array<SqlParameter^>^ ApptParams(Appointment^ a)
    {
        Object^ sv = nullptr;
        if (a->ServiceId > 0) sv = a->ServiceId;
        Object^ st = nullptr;
        if (!String::IsNullOrEmpty(a->StaffId)) st = a->StaffId;
        return gcnew array<SqlParameter^> {
            P(L"@id", a->Id), P(L"@n", a->CustomerName), P(L"@e", a->CustomerEmail), P(L"@p", a->Phone),
                P(L"@sv", sv), P(L"@st", st), P(L"@t", a->AppointmentTime), P(L"@status", a->Status), P(L"@notes", a->Notes) };
    }

    // a staff member cannot have two active appointments at the same time
    static void CheckConflict(Appointment^ a)
    {
        if (String::IsNullOrEmpty(a->StaffId) || a->Status == L"Cancelled") return;
        int n = Count(L"SELECT COUNT(*) FROM dbo.Appointments WHERE StaffId=@st AND AppointmentTime=@t "
            L"AND Status<>N'Cancelled' AND AppointmentId<>@id",
            gcnew array<SqlParameter^> { P(L"@st", a->StaffId), P(L"@t", a->AppointmentTime), P(L"@id", a->Id) });
        if (n > 0) throw gcnew Exception(L"The selected staff member already has an appointment at that time.");
    }

    // every booked customer is also kept in the Customers table
    static void EnsureCustomer(String^ name, String^ phone, String^ email)
    {
        if (Exists(L"Customers", L"Phone", phone)) return;
        String^ id = NextId(L"Customers", L"CustomerId", L"C");
        Exec(L"INSERT INTO dbo.Customers (CustomerId, Name, Phone, Email) VALUES (@id, @n, @p, @e)",
            gcnew array<SqlParameter^> { P(L"@id", id), P(L"@n", name), P(L"@p", phone), P(L"@e", email) });
    }
};