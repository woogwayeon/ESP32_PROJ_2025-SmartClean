<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ page import="java.sql.*, java.io.*, jakarta.servlet.*, jakarta.servlet.annotation.*, jakarta.servlet.http.*" %>
<%
	response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");

    String userId = (session != null) ? (String)session.getAttribute("User_ID") : null;

    String APT_UNIT = "";
    String APT_NUM = "";
    String User_Name = "";
    String User_Phone = "";

    if (userId == null) 
    {
        out.println("<script>");
        out.println("alert('로그인 정보가 없습니다.');");
        out.println("location.href='index.jsp';");
        out.println("</script>");
        return;
    }

    Connection conn = null;
    PreparedStatement pstmt = null;
    ResultSet rs = null;
    
	String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
	String dbID = "MY_DBID";
	String dbPW = "MY_DBPW";

    try 
    {
        Class.forName("com.mysql.cj.jdbc.Driver");
        conn = DriverManager.getConnection(dbURL, dbID, dbPW);

        String sql = "SELECT * FROM House WHERE User_ID = ?";
        pstmt = conn.prepareStatement(sql);
        pstmt.setString(1, userId);
        rs = pstmt.executeQuery();

        if (rs.next()) 
        {
            APT_UNIT = rs.getString("APT_UNIT");
            APT_NUM = rs.getString("APT_NUM");
            User_Name = rs.getString("User_Name");
            User_Phone = rs.getString("User_Phone");
        } 
        else 
        {
            out.println("<script>");
            out.println("alert('회원 정보를 불러올 수 없습니다.');");
            out.println("history.back();");
            out.println("</script>");
            return;
        }
    } 
    catch (Exception e) 
    {
        out.println("<script>");
        out.println("alert('데이터를 불러오는 중 오류가 발생했습니다.');");
        out.println("history.back();");
        out.println("</script>");
        return;
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
            System.out.println("register-changeInfo-user close error: " + e);
        }
    }

 	// 전화번호
    String phone1 = "";
    String phone2 = "";
    String phone3 = "";

    if (User_Phone != null && !User_Phone.trim().equals("")) 
    {
        // 하이픈 제거 후 숫자만 남김
        String cleanPhone = User_Phone.replaceAll("[^0-9]", "");

        if (cleanPhone.length() >= 10 && cleanPhone.length() <= 11) 
        {
            phone1 = cleanPhone.substring(0, 3);

            if (cleanPhone.length() == 10) 
            {
                phone2 = cleanPhone.substring(3, 6);
                phone3 = cleanPhone.substring(6);
            } 
            else 
            {
                phone2 = cleanPhone.substring(3, 7);
                phone3 = cleanPhone.substring(7);
            }
        }
    }
%>
<!DOCTYPE html>
<html lang="ko">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Smart Clean : 개인 비밀번호 변경</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/detailPage.css">
	<link rel="stylesheet" href="./style/register.css">
	<link rel="stylesheet" href="./style/register-changePw.css">
</head>
<body>

	<div class="wrap">
		<div>
			<h1>Edit Password</h1>
			<div class="logout">
				<a href="smart-user.jsp">Home</a>
			</div>	
			<div class="logout">
				<a href="LogoutServlet">Logout</a>
			</div>
		</div>
		<div class="inner-wrap">
			<header>
				<h2>Smart Clean System 비밀번호 변경</h2>
				<p><%= APT_UNIT %>동 <%= APT_NUM %>호</p>
			</header>

			<main>

				<div class="floor00">
					
					<article class="register-inner">
						<form method="post" action="UpdatePasswordServlet">
							
							<div class="addr">
								<div class="txt-alert">
									동 / 호수
								</div>
								<div class="addr2">
									<div class="unit">
										<input type="text" name="apt_unit" value="<%= APT_UNIT %>" disabled>
									</div>
									<div class="unit-num">
										<input type="text" name="apt_num" value="<%= APT_NUM %>" disabled>
									</div>	
								</div>
							</div>

							<div class="id">
								<div class="txt-alert">
									<p>ID</p>
								</div>
								<div class="inp">
									<input type="text" name="user_id" value="<%= userId %>" disabled>
								</div>
							</div>

							<div class="name">
								<div class="txt-alert">
									<p>세대주 이름</p>
								</div>
								<div class="inp">
									<input type="text" name="user_name" value="<%= User_Name %>" disabled>
								</div>
							</div>

							<div class="phone">
								<div class="txt-alert">
									<p>세대주 휴대폰번호</p>
								</div>
								<div class="inp-phone">
									<input type="text" name="user_phone" value="<%= phone1 %>" disabled>
									<input type="text" name="user_phone2" value="<%= phone2 %>" disabled>
									<input type="text" name="user_phone3" value="<%= phone3 %>" disabled>
								</div>
							</div>

							<div class="pw">
								<div class="txt-alert">
									<p>비밀번호</p>
								</div>
								<div class="inp">
									<input type="password" name="user_pw" placeholder="비밀번호 입력">
								</div>
							</div>

							<div class="pw">
								<div class="txt-alert">
									<p>비밀번호 확인</p>
								</div>
								<div class="inp">
									<input type="password" name="user_pw_ck" placeholder="비밀번호 확인">
								</div>
							</div>

							<div class="pw">
								<div class="txt-alert">
									<p></p>
								</div>
								<div class="inp">
									<input type="button" value="비밀번호 확인">
								</div>
							</div>

							<div class="id">
								<div class="txt-alert pw-alert">
									<p>
										비밀번호는 영문 대소문자, 숫자, 특수문자 4자리 이상, 26자 이하 입력 가능합니다
									</p>
								</div>
							</div>

							<div class="submit-btn">
								<input type="submit" value="제출하기">
							</div>
						</form>
					</article>

				</div>

				<div class="floor03">
                    <a href="#" class="noti">Smart Clean System</a>
                    <a href="smart-user.jsp" class="ask">뒤로 돌아가기</a>
                </div>

			</main>
		</div>

		<footer>
			<div class="footer-inner">
				<p>주소 : 강원도 춘천시 동산면 원창리 폴리폴리 네이쳐빌리지아파트</p>
				<p>전화번호 : 02-000-0000</p>
				<p>email : polypoly@aisw.co.kr</p>
			</div>
			<p>&copy; 2025 Seoyeon. All rights reserved.</p>
		</footer>
	</div>
	
	<script src="./js/passwordCK.js"></script>
</body>
</html>