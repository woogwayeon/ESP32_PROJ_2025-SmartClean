<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ page import="java.sql.*" %>
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <title>회원 목록</title>
    <link rel="stylesheet" href="./style/default.css">
    <link rel="stylesheet" href="./style/paidCheck.css">
    <link rel="stylesheet" href="./style/memberList.css">
</head>
<body>
    <div class="wrap">
        <h2>전체 회원 조회</h2>

        <%
            Connection conn = null;
            Statement stat = null;
            ResultSet rset = null;

            String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
            String dbId = "MY_DBID";
            String dbPw = "MY_DBPW";

            String sql = "SELECT * FROM House";

            try {
                Class.forName("com.mysql.cj.jdbc.Driver");
                conn = DriverManager.getConnection(dbURL, dbId, dbPw);
                stat = conn.createStatement();
                rset = stat.executeQuery(sql);
        %>

        <table>
            <thead>
                <tr>
                    <th>세대 ID</th>
                    <th>동</th>
                    <th>호수</th>
                    <th>이름</th>
                    <th>전화번호</th>
                    <th>로그인 ID</th>
                    <th>비밀번호</th>
                    <th>권한</th>
                </tr>
            </thead>
            <tbody>
                <%
                    while (rset.next()) {
                %>
                <tr>
                    <td><%= rset.getInt("House_ID") %></td>
                    <td><%= rset.getString("APT_UNIT") %></td>
                    <td><%= rset.getString("APT_NUM") %></td>
                    <td><%= rset.getString("User_Name") %></td>
                    <td><%= rset.getString("User_Phone") %></td>
                    <td><%= rset.getString("User_ID") %></td>
                    <td><%= rset.getString("User_PW") %></td>
                    <td><%= rset.getString("ROLE") %></td>
                </tr>
                <%
                    }
                %>
            </tbody>
        </table>

        <%
            } catch (Exception e) {
                out.println("<p style='color:red;'>데이터를 불러오는 중 오류가 발생했습니다.</p>");
                e.printStackTrace();
            } finally {
                try {
                    if (rset != null) rset.close();
                    if (stat != null) stat.close();
                    if (conn != null) conn.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        %>
		
		<div class="list-btn">
			<button class="back-btn" onclick="location.href='smart-manager.jsp'">메인 페이지로 이동</button>
        	<button class="back-btn" onclick="location.href='register-manager.jsp'">회원등록 페이지로 이동</button>
		</div>
		
    </div>
</body>
</html>