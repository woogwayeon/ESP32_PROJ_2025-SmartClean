package DAO;

import java.sql.*;

public class MemberDAO {

	// singleton
    private static MemberDAO instance = new MemberDAO();

    private Connection conn;
    private final String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
    private final String dbId = "MY_DBID";
    private final String dbPw = "MY_DBPW";

    private MemberDAO() 
    {
        try 
        {
            Class.forName("com.mysql.cj.jdbc.Driver");
            conn = DriverManager.getConnection(dbURL, dbId, dbPw);
            System.out.println("DB 연결 성공 (Singleton)");
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }
    }

    public static MemberDAO getInstance() 
    {
        return instance;
    }

    public Connection getConnection() 
    {
        return conn;
    }
    
    public void close(Connection conn, PreparedStatement pstmt, ResultSet rs) 
    {
        try 
        {
            if (rs != null) rs.close();
            if (pstmt != null) pstmt.close();
            if (conn != null) conn.close();
        } 
        catch (Exception e) 
        {
            System.out.println("MemberDAO close error: " + e);
        }
    }

    // 로그인 검증 메서드
    public ResultSet loginCheck(String id, String pw) throws SQLException {
        String sql = "SELECT * FROM House WHERE User_ID = ? AND User_PW = ?";
        PreparedStatement pstmt = conn.prepareStatement(sql);
        pstmt.setString(1, id);
        pstmt.setString(2, pw);
        return pstmt.executeQuery();
    }

    // 회원 등록 메서드 (register-manager.jsp용)
    public int insertMember(String APT_UNIT, String APT_NUM, String User_Name, String User_Phone,
                            String User_ID, String User_PW, String Member_NUM, String ROLE) throws SQLException {

        String sql = "INSERT INTO House (APT_UNIT, APT_NUM, User_Name, User_Phone, User_ID, User_PW, Member_NUM, ROLE) "
                   + "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
        PreparedStatement pstmt = conn.prepareStatement(sql);
        pstmt.setString(1, APT_UNIT);
        pstmt.setString(2, APT_NUM);
        pstmt.setString(3, User_Name);
        pstmt.setString(4, User_Phone);
        pstmt.setString(5, User_ID);
        pstmt.setString(6, User_PW);
        pstmt.setString(7, Member_NUM);
        pstmt.setString(8, ROLE);

        int result = pstmt.executeUpdate();
        pstmt.close();
        return result;
    }
}