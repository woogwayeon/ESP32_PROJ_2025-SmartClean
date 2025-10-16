package controller;

import jakarta.servlet.*;
import jakarta.servlet.annotation.*;
import jakarta.servlet.http.*;
import java.io.*;
import java.sql.*;


@WebServlet("/UpdatePasswordServlet")
public class UpdatePasswordServlet extends HttpServlet 
{
    private static final long serialVersionUID = 1L;

    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        request.setCharacterEncoding("UTF-8");
        response.setContentType("text/html; charset=UTF-8");

        java.io.PrintWriter out = response.getWriter();
        HttpSession session = request.getSession(false);

        if (session == null || session.getAttribute("User_ID") == null) 
        {
            out.println("<script>");
            out.println("alert('로그인 세션이 만료되었습니다. 다시 로그인해주세요.');");
            out.println("location.href='index.jsp';");
            out.println("</script>");
            return;
        }

        String userId = (String) session.getAttribute("User_ID");
        String user_pw = request.getParameter("user_pw");
        String user_pw_ck = request.getParameter("user_pw_ck");

        if (user_pw == null || user_pw_ck == null || user_pw.trim().equals("") || user_pw_ck.trim().equals("")) 
        {
            out.println("<script>");
            out.println("alert('비밀번호를 모두 입력해주세요.');");
            out.println("history.back();");
            out.println("</script>");
            return;
        }

        if (!user_pw.equals(user_pw_ck)) 
        {
            out.println("<script>");
            out.println("alert('비밀번호 확인이 일치하지 않습니다.');");
            out.println("history.back();");
            out.println("</script>");
            return;
        }

        Connection conn = null;
        PreparedStatement pstmt = null;

        String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
        String dbID = "MY_DBID";
        String dbPW = "MY_DBPW";

        try 
        {
            Class.forName("com.mysql.cj.jdbc.Driver");
            conn = DriverManager.getConnection(dbURL, dbID, dbPW);

            String sql = "UPDATE House SET User_Pw = ? WHERE User_ID = ?";
            pstmt = conn.prepareStatement(sql);
            pstmt.setString(1, user_pw);
            pstmt.setString(2, userId);

            int result = pstmt.executeUpdate();

            if (result > 0) 
            {
                out.println("<script>");
                out.println("alert('비밀번호가 성공적으로 변경되었습니다. 다시 로그인해주세요.');");
                out.println("location.href='index.jsp';");
                out.println("</script>");
            } 
            else 
            {
                out.println("<script>");
                out.println("alert('비밀번호 변경에 실패했습니다.');");
                out.println("history.back();");
                out.println("</script>");
            }
        } 
        catch (Exception e) 
        {
            out.println("<script>");
            out.println("alert('서버 오류가 발생했습니다: " + e.getMessage().replace("'", "") + "');");
            out.println("history.back();");
            out.println("</script>");
        } 
        finally 
        {
            try 
            {
                if (pstmt != null) pstmt.close();
                if (conn != null) conn.close();
            } 
            catch (Exception e) 
            {
                System.out.println("UpdatePasswordServlet close error: " + e);
            }
        }
    }
}