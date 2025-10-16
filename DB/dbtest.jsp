<%@ page import="java.sql.*" %>
<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%
String url = "jdbc:mysql://localhost:3306/iotdb";
String user = "DATABASE_ID";
String password = "DATABASE_PW";

try 
{
    Class.forName("com.mysql.cj.jdbc.Driver");
    Connection conn = DriverManager.getConnection(url, user, password);

    out.println("DB 연결 성공!<br>");
    Statement stmt = conn.createStatement();
    ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM test");

    if (rs.next()) 
    {
        out.println("test db line : " + rs.getInt(1));
    }
    conn.close();
} 
catch (Exception e) 
{
    out.println("DB 연결 실패: " + e.getMessage());
}
%>
