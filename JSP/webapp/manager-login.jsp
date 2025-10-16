<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html lang="ko">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Smart Clean : 관리자 로그인</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/login.css">
	<link rel="stylesheet" href="./style/manager-login.css">
</head>
<body>
	<div class="wrap">
		<div class="inner-wrap">
			<div class="right-box"></div>

			<div class="left-box">
				<form action="LoginServlet" method="post" name="frm">
					<input type="hidden" name="loginTarget" value="admin">
					<div class="login-title">
						<h5>Manager Login</h5>
					</div>
					<input type="text" name="inpId" placeholder="ID">
					<input type="password" name="inpPw" placeholder="Password">
				
					<input type="submit" value="로그인" onclick="return check()">
					<a href="index.jsp">
						사용자 프로그램 로그인으로 돌아가기
					</a>
				</form>	
			</div>
		</div>
	</div>
	<footer>
		<p>&copy; 2025 Seoyeon. All rights reserved.</p>
	</footer>
	<script src="./js/loginCheck.js"></script>
</body>
</html>