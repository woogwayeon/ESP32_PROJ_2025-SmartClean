package controller;

import jakarta.servlet.*;
import jakarta.servlet.annotation.*;
import jakarta.servlet.http.*;
import java.io.*;
import java.sql.*;

@WebServlet("/MemberInsertServlet")
public class MemberInsertServlet extends HttpServlet {
    private static final long serialVersionUID = 1L;

    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        request.setCharacterEncoding("UTF-8");
        response.setContentType("text/html; charset=UTF-8");

        // 파라미터
        String APT_UNIT = request.getParameter("APT_UNIT");
        String APT_NUM = request.getParameter("APT_NUM");
        String User_Name = request.getParameter("User_Name");
        String User_ID = request.getParameter("User_ID");
        String User_PW = request.getParameter("User_PW");
        String ROLE = request.getParameter("ROLE");
        String Member_NUM = request.getParameter("Member_NUM");

        // 전화번호
        String User_Phone1 = request.getParameter("User_Phone");
        String User_Phone2 = request.getParameter("User_Phone2");
        String User_Phone3 = request.getParameter("User_Phone3");

        String User_Phone = "";
        
        if (User_Phone1 != null && User_Phone2 != null && User_Phone3 != null) 
        {
            User_Phone = User_Phone1 + "-" + User_Phone2 + "-" + User_Phone3;
        }

        Connection conn = null;
        PreparedStatement pstmt = null;

        // DB 연결
        String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
        String dbId = "MY_DBID";
        String dbPw = "MY_DBPW";

        try 
        {
            Class.forName("com.mysql.cj.jdbc.Driver");
            conn = DriverManager.getConnection(dbURL, dbId, dbPw);

            // INSERT (House 테이블)
            String sql = "INSERT INTO House (APT_UNIT, APT_NUM, User_Name, User_Phone, User_ID, User_PW, Member_NUM, ROLE) "
                       + "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
            pstmt = conn.prepareStatement(sql);
            pstmt.setString(1, APT_UNIT);
            pstmt.setString(2, APT_NUM);
            pstmt.setString(3, User_Name);
            pstmt.setString(4, User_Phone);
            pstmt.setString(5, User_ID);
            pstmt.setString(6, User_PW);
            pstmt.setString(7, Member_NUM);
            pstmt.setString(8, ROLE);

            int result = pstmt.executeUpdate();

            if (result > 0) 
            {
                response.getWriter().println("<script>");
                response.getWriter().println("alert('등록이 완료되었습니다.');");
                response.getWriter().println("location.href='member_list.jsp';");
                response.getWriter().println("</script>");
            } 
            else 
            {
                response.getWriter().println("<script>");
                response.getWriter().println("alert('등록에 실패했습니다. 다시 시도해주세요.');");
                response.getWriter().println("history.back();");
                response.getWriter().println("</script>");
            }

        } 
        catch (Exception e) 
        {
            System.out.println("MemberInsertServlet error: " + e);
            response.getWriter().println("<script>");
            response.getWriter().println("alert('데이터베이스 오류가 발생했습니다.');");
            response.getWriter().println("history.back();");
            response.getWriter().println("</script>");
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
                System.out.println("MemberInsertServlet error: " + e);
            }
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        doPost(request, response);
    }
}