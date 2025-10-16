<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html lang="ko">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Smart Clean : 회원등록</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/detailPage.css">
	<link rel="stylesheet" href="./style/register.css">
</head>
<body>

	<div class="wrap">
		<div>
			<h1>Register</h1>
			<div class="logout">
				<a href="smart-manager.jsp">Home</a>
			</div>	
			<div class="logout">
				<a href="index.jsp">Logout</a>
			</div>
		</div>
		<div class="inner-wrap">
			<header>
				<h2>Smart Clean System 회원등록</h2>
				<p>관리자 고서연님 환영합니다!</p>
			</header>

			<main>

				<div class="floor00">
					
					<article class="register-inner">
						<form action="MemberInsertServlet" method="post">
							
							<div class="addr">
								<div class="txt-alert">
									동 / 호수
								</div>
								<div class="addr2">
									<div class="unit">
										<input type="text" name="APT_UNIT" placeholder="101" oninput="this.value = this.value.replace(/[^0-9]/g,'').slice(0,4);" inputmode="numeric">
									</div>
									<div class="unit-num">
										<input type="text" name="APT_NUM" placeholder="1404" oninput="this.value = this.value.replace(/[^0-9]/g,'').slice(0,4);" inputmode="numeric">
									</div>	
								</div>
							</div>

							<div class="id">
								<div class="txt-alert">
									<p>
										ID 예시 : 101동 902호면 
										<span class="bold-color">101-902</span>
									</p>
									<p>반드시 양식을 지켜서 입력하세요</p>
								</div>
								<div class="inp">
									<input type="text" name="User_ID" placeholder="ID" placeholder="101-902" oninput="this.value = this.value.replace(/[^0-9\-]/g,'').slice(0,9);" inputmode="numeric">
								</div>
							</div>
						
							<div class="pw">
								<div class="txt-alert">
									<p>비밀번호</p>
								</div>
								<div class="inp">
									<input type="text" name="User_PW" value="1111" readonly>
								</div>
							</div>

							<div class="mem-num">
								<div class="txt-alert">
									<p>세대 구성원 수</p>
								</div>
								<div class="inp">
									<input type="text" name="Member_NUM" pattern="[0-9]*" inputmode="numeric" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="2" placeholder="숫자만 입력하세요">
								</div>
							</div>

							<div class="name">
								<div class="txt-alert">
									<p>세대주 이름</p>
								</div>
								<div class="inp">
									<input type="text" name="User_Name" placeholder="세대주 이름">
								</div>
							</div>


							<div class="phone">
								<div class="txt-alert">
									<p>세대주 휴대폰번호</p>
								</div>
								<div class="inp-phone">
									<input type="text" name="User_Phone" value="010">
									<input type="text" name="User_Phone2" pattern="[0-9]*" inputmode="numeric" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="4" placeholder="0000">
									<input type="text" name="User_Phone3" pattern="[0-9]*" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="4" inputmode="numeric" placeholder="0000">
								</div>
							</div>
							
							<div class="rol">
							    <div class="txt-alert">
							        <label for="role">사용자 권한</label>	
							    </div>
							    
							    <div class="select-role">
							        <select id="role" name="role" onchange="userRole()">
							            <option value="USER">USER</option>
							            <option value="ADMIN">ADMIN</option>
							            <option value="EMPTY">EMPTY</option>
							        </select>	
							        <!-- 선택된 값 자동 복사용 hidden input -->
							        <input type="hidden" name="ROLE" id="ROLE_hidden">
							    </div>
							</div>
							
							<script>
							    function userRole() {
							        document.getElementById("ROLE_hidden").value =
							            document.getElementById("role").value;
							    }
							    document.addEventListener("DOMContentLoaded", userRole);
							</script>
							
							<div class="submit-btn">
								<input type="submit" value="회원정보를 등록합니다">
							</div>
						</form>
					</article>

				</div>

				<div class="floor03">
                    <a href="#" class="noti">
                        회원정보 수정
                    </a>
                    <a href="smart-manager.jsp" class="ask">
                        뒤로 돌아가기
                    </a>
                </div>

			</main>
		</div>

		<footer>
			<div class="footer-inner">
				<p>
					주소 : 강원도 춘천시 동산면 원창리 폴리폴리 네이쳐빌리지아파트
				</p>
				<p>
					전화번호 : 02-000-0000
				</p>
				<p>
					email : polypoly@aisw.co.kr
				</p>
			</div>
			<p>&copy; 2025 Seoyeon. All rights reserved.</p>
		</footer>
	</div>
</body>
</html>