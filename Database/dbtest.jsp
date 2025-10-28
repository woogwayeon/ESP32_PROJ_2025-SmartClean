<%@ page import="java.sql.*" %>
<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%
String url = "jdbc:mysql://localhost:3306/iotdb";
String user = "root";
String password = "poly";

try 
{
    Class.forName("com.mysql.cj.jdbc.Driver");
    Connection conn = DriverManager.getConnection(url, user, password);

    out.println("DB 연결 성공!<br>");
    Statement stmt = conn.createStatement();
    ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM WasteRecord");

    if (rs.next()) 
    {
        out.println("현재 WasteRecord 레코드 수: " + rs.getInt(1));
    }
    conn.close();
} 
catch (Exception e) 
{
    out.println("DB 연결 실패: " + e.getMessage());
}
%>