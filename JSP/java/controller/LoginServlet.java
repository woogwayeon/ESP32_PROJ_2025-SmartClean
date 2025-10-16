package controller;

import jakarta.servlet.*;
import jakarta.servlet.annotation.*;
import jakarta.servlet.http.*;
import java.io.*;

import java.sql.*;

@WebServlet("/LoginServlet")
public class LoginServlet extends HttpServlet {
    private static final long serialVersionUID = 1L;

    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        request.setCharacterEncoding("UTF-8");
        response.setContentType("text/html; charset=UTF-8");

        String id = request.getParameter("inpId");
        String pw = request.getParameter("inpPw");

        Connection conn = null;
        PreparedStatement pstmt = null;
        ResultSet rs = null;

        // DB 연결
        String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
        String dbId = "MY_DBID";
        String dbPw = "MY_DBPW";

        try 
        {
            Class.forName("com.mysql.cj.jdbc.Driver");
            conn = DriverManager.getConnection(dbURL, dbId, dbPw);

            // House 테이블 확인
            String sql = "SELECT * FROM House WHERE User_ID = ? AND User_PW = ?";
            pstmt = conn.prepareStatement(sql);
            pstmt.setString(1, id);
            pstmt.setString(2, pw);
            rs = pstmt.executeQuery();

            if (rs.next()) 
            {
                String role = rs.getString("ROLE");
                String name = rs.getString("User_Name");

                HttpSession session = request.getSession();
                session.setAttribute("User_ID", id);
                session.setAttribute("User_Name", name);
                session.setAttribute("ROLE", role);

                // 관리자/사용자 로그인 구분
                if ("ADMIN".equalsIgnoreCase(role)) 
                {
                    response.getWriter().println("<script>");
                    response.getWriter().println("alert('관리자 로그인 성공!');");
                    response.getWriter().println("location.href='smart-manager.jsp';");
                    response.getWriter().println("</script>");
                } 
                else if ("USER".equalsIgnoreCase(role)) 
                {
                    response.getWriter().println("<script>");
                    response.getWriter().println("alert('사용자 로그인 성공!');");
                    response.getWriter().println("location.href='smart-user.jsp';");
                    response.getWriter().println("</script>");
                } 
                else 
                {
                    response.getWriter().println("<script>");
                    response.getWriter().println("alert('권한이 없는 계정입니다.');");
                    response.getWriter().println("history.back();");
                    response.getWriter().println("</script>");
                }

            } 
            else 
            {
                // ID/PW 불일치
                response.getWriter().println("<script>");
                response.getWriter().println("alert('아이디 또는 비밀번호가 올바르지 않습니다.');");
                response.getWriter().println("history.back();");
                response.getWriter().println("</script>");
            }

        } 
        catch (Exception e) 
        {
            e.printStackTrace();
            response.getWriter().println("<script>");
            response.getWriter().println("alert('서버 오류가 발생했습니다.');");
            response.getWriter().println("history.back();");
            response.getWriter().println("</script>");
        } 
        finally 
        {
            try 
            {
                if (rs != null) rs.close();
                if (pstmt != null) pstmt.close();
                if (conn != null) conn.close();
            } 
            catch (Exception e) 
            {
            	System.out.println("loginServlet error: " + e);
            }
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException 
    {
        doPost(request, response);
    }
}
