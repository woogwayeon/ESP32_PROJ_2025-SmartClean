<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ page import="
				java.sql.*, 
				java.io.*, 
				java.util.*,
				java.time.LocalDate, 
				java.time.format.DateTimeFormatter, 
				jakarta.servlet.*, 
				jakarta.servlet.annotation.*, 
				jakarta.servlet.http.*,
				com.google.gson.Gson				
"%>
<%
	response.setContentType("text/html; charset=UTF-8");
	request.setCharacterEncoding("UTF-8");

	String userId = null;
	String role = null;

	if (session != null) 
	{
		userId = (String)session.getAttribute("User_ID");
		role = (String)session.getAttribute("User_ROLE");
	}
	
	Connection conn = null;
	PreparedStatement pstmt = null;
	ResultSet rs = null;

	String dbURL = "jdbc:mysql://127.0.0.1:3306/iotdb";
	String dbID = "MY_DBID";
	String dbPW = "MY_DBPW";
	
	String sql;
	
	int todayTotal = 0;
	int monthTotalBill = 0;
	int prevMonthBill = 0;
	int unpaidCount = 0;
	int monthWaste = 0;
	int avgWaste = 0;

	List<String> weekLabels = new ArrayList<String>();
	List<Integer> weekData = new ArrayList<Integer>();
	List<String> monthLabels = new ArrayList<String>();
	List<Integer> monthData = new ArrayList<Integer>();
	
	try 
	{
		Class.forName("com.mysql.cj.jdbc.Driver");
		conn = DriverManager.getConnection(dbURL, dbID, dbPW);

		// 오늘의 폐기량
		sql = "SELECT IFNULL(SUM(Amount),0) AS TodayTotal FROM WasteRecord WHERE Record_DATE = CURDATE()";
		System.out.println("SQL(todayTotal): " + sql);
		
		pstmt = conn.prepareStatement(sql);
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			todayTotal = rs.getInt("TodayTotal");
		}
		
		System.out.println("todayTotal = " + todayTotal);
		
		rs.close();
		pstmt.close();
		
		

		// 이번달 누적 청구액
		sql = "SELECT IFNULL(SUM(Total_Bill),0) AS MonthBill FROM Billing WHERE Period = ?";
		
		pstmt = conn.prepareStatement(sql);
		pstmt.setString(1, LocalDate.now().format(DateTimeFormatter.ofPattern("yyyy-MM")));
		System.out.println("SQL(monthTotalBill): " + pstmt.toString());
		
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			monthTotalBill = rs.getInt("MonthBill");
		}
		
		System.out.println("monthTotalBill = " + monthTotalBill);
		
		rs.close();
		pstmt.close();

		
		
		// 지난달 청구액 (문자열 비교)
		sql = "SELECT IFNULL(SUM(Total_Bill),0) AS PrevBill FROM Billing WHERE Period = ?";
		
		pstmt = conn.prepareStatement(sql);
		pstmt.setString(1, LocalDate.now().minusMonths(1).format(DateTimeFormatter.ofPattern("yyyy-MM")));
		
		System.out.println("SQL(prevMonthBill): " + pstmt.toString());
		
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			prevMonthBill = rs.getInt("PrevBill");
		}
		
		System.out.println("prevMonthBill = " + prevMonthBill);
		
		rs.close();
		pstmt.close();

		
		
		// 미납 세대수
		sql = "SELECT COUNT(*) AS Unpaid FROM Billing WHERE Status != 'PAID' AND DATE_FORMAT(Period, '%Y-%m') = DATE_FORMAT(CURDATE(), '%Y-%m')";
		System.out.println("SQL(unpaidCount): " + sql);
		
		pstmt = conn.prepareStatement(sql);
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			unpaidCount = rs.getInt("Unpaid");
		}
		
		System.out.println("unpaidCount = " + unpaidCount);
		
		rs.close();
		pstmt.close();

		
		
		// 이번달 전체 누적 폐기량
		sql = "SELECT IFNULL(SUM(Amount),0) AS MonthWaste FROM WasteRecord WHERE DATE_FORMAT(Record_DATE, '%Y-%m') = DATE_FORMAT(CURDATE(), '%Y-%m')";
		System.out.println("SQL(monthWaste): " + sql);
		
		pstmt = conn.prepareStatement(sql);
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			monthWaste = rs.getInt("MonthWaste");
		}
		
		System.out.println("monthWaste = " + monthWaste);
		
		rs.close();
		pstmt.close();

		
		
		// 세대별 평균
		sql = "SELECT COUNT(*) AS UserCount FROM House WHERE ROLE='USER'";
		
		pstmt = conn.prepareStatement(sql);
		rs = pstmt.executeQuery();
		
		if (rs.next()) 
		{
			int userCount = rs.getInt("UserCount");
			
			if (userCount > 0) 
			{
				avgWaste = monthWaste / userCount;
			}
			
			System.out.println("userCount = " + userCount + ", avgWaste = " + avgWaste);
		}
		
		rs.close();
		pstmt.close();

		// 최근 7일 폐기량
		String weekSql = "SELECT DATE_FORMAT(Record_DATE, '%m-%d') AS d, SUM(Amount) AS total "
		               + "FROM WasteRecord WHERE Record_DATE >= DATE_SUB(CURDATE(), INTERVAL 6 DAY) "
		               + "GROUP BY d ORDER BY d";
		               
		System.out.println("SQL(weekChart): " + weekSql);
		
		pstmt = conn.prepareStatement(weekSql);
		rs = pstmt.executeQuery();
		
		while (rs.next()) 
		{
			weekLabels.add(rs.getString("d"));
			weekData.add(rs.getInt("total"));
		}
		
		System.out.println("weekLabels = " + weekLabels);
		System.out.println("weekData = " + weekData);
		
		rs.close();
		pstmt.close();

		
		
		// 최근 6개월 청구액
		String monthSql = "SELECT Period AS p, SUM(Total_Bill) AS total "
		                 + "FROM Billing "
		                 + "WHERE Period >= DATE_FORMAT(DATE_SUB(CURDATE(), INTERVAL 6 MONTH), '%Y-%m') "
		                 + "GROUP BY Period ORDER BY Period ASC";
		                 
        System.out.println("SQL(monthChart): " + monthSql);
		
        pstmt = conn.prepareStatement(monthSql);
		rs = pstmt.executeQuery();
		
		while (rs.next()) 
		{
			monthLabels.add(rs.getString("p"));
			monthData.add(rs.getInt("total"));
		}
		
		System.out.println("monthLabels = " + monthLabels);
		System.out.println("monthData = " + monthData);
		
		rs.close();
		pstmt.close();
		
	} 
	catch (Exception e) 
	{
		System.out.println("smart-manager DB error: " + e);
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
			System.out.println("smart-manager close error: " + e);
		}
	}

	LocalDate today = LocalDate.now();
	String yearText = today.format(DateTimeFormatter.ofPattern("yyyy년"));
	String dayText = today.format(DateTimeFormatter.ofPattern("MM월 dd일(E)"));
	String prevMonthText = today.minusMonths(1).format(DateTimeFormatter.ofPattern("M"));
	
	String weekLabelsJson = new Gson().toJson(weekLabels);
	String weekDataJson = new Gson().toJson(weekData);
	String monthLabelsJson = new Gson().toJson(monthLabels);
	String monthDataJson = new Gson().toJson(monthData);
%>
<!DOCTYPE html>
<html lang="ko">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Smart Clean : 관리자 상세보기</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/detailPage.css">
</head>
<body>

	<div class="wrap">
		<div>
			<h1>Smart Clean System For Manager</h1>
			<div class="logout">
				<a href="register-manager.jsp">회원등록 및 변경</a>
			</div>	
			<div class="logout">
				<a href="manager-login.jsp">Logout</a>
			</div>
		</div>
		<div class="inner-wrap">
			<header>
				<h2>폴리폴리 네이쳐빌리지 아파트</h2>
				<p>관리자님 환영합니다!</p>
			</header>

			<main>

				<div class="floor00">
					
					<article class="today-date">
						<h4><%= yearText %></h4>
						<h3><%= dayText %></h3>
					</article>

					<article class="today-total">
						<h4>오늘의 폐기량</h4>
						<h3><%= todayTotal %>g</h3>
					</article>

					<article class="today-bill">
						<h4>이번달 누적 청구액</h4>
						<h3><%= monthTotalBill %>원</h3>
					</article>

				</div>

				<div class="floor01">
					
					<article class="month-bill">
						<p class="paid-check">납부완료</p>
						<h4>
							<%= prevMonthText %>월 전 세대 청구금액
						</h4>
						<h3 class="month-pay">
							<%= prevMonthBill %>원
						</h3>
						<div class="paid-lim">
							<p>미납 세대수</p>
							<p><%= unpaidCount %>세대</p>
						</div>
						<button>
							<a href="paidCheck-manager.jsp">
								전체 납부내역 확인하기
							</a>
						</button>
					</article>
					
					<article class="week-bill">
						<div class="week-chart">
							<canvas id="weekChart"></canvas>
						</div>
					</article>

				</div>
				
				<div class="floor02">
					
					<article class="cumul">
						
						<div class="amount">
							<h4>이번달 전체 누적 폐기량</h4>
							<h3><%= monthWaste %>g</h3>							
						</div>

						<div class="billing">
							<h4>세대별 평균 폐기량</h4>
							<h3>일 <%= avgWaste %>g</h3>
						</div>
						
					</article>

					<article class="month-total">
						<div class="month-chart">
							<canvas id="monthChart"></canvas>
						</div>
					</article>
				</div>

				<div class="floor03">
					<a href="#" class="noti">
						공지사항 및 문의글
					</a>
					<a href="member_list.jsp" class="ask">
						회원정보 조회하기
					</a>
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
	
	<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

	<script>
		console.log("weekLabels =", <%= weekLabelsJson %>);
		console.log("weekData =", <%= weekDataJson %>);
		console.log("monthLabels =", <%= monthLabelsJson %>);
		console.log("monthData =", <%= monthDataJson %>);

		const weekLabels = <%= weekLabelsJson %>;
		const weekData = <%= weekDataJson %>;
		const monthLabels = <%= monthLabelsJson %>;
		const monthData = <%= monthDataJson %>;
	</script>

	<script src="./js/detailChart-manager.js"></script>
</body>
</html>