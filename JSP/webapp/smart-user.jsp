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
    if (session != null) 
    {
        userId = (String) session.getAttribute("User_ID");
    }

    if (userId == null || userId.trim().equals("")) 
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

    int todayAmount = 0;
    int monthAmount = 0;
    int totalBill = 0;
    String period = "-";
    String status = "-";
    String dueDate = "-";
    String APT_UNIT = "";
    String APT_NUM = "";
    String paidText = "";
    String periodText = "-";

    // 차트용 데이터
    List<String> weekLabels = new ArrayList<String>();
    List<Integer> weekData = new ArrayList<Integer>();
    List<String> monthLabels = new ArrayList<String>();
    List<Integer> monthData = new ArrayList<Integer>();

    try 
    {
        Class.forName("com.mysql.cj.jdbc.Driver");
        conn = DriverManager.getConnection(dbURL, dbID, dbPW);

        String sql = "SELECT House_ID, APT_UNIT, APT_NUM FROM House WHERE User_ID = ?";
        pstmt = conn.prepareStatement(sql);
        pstmt.setString(1, userId);
        rs = pstmt.executeQuery();

        int houseId = 0;
        
        if (rs.next()) 
        {
            houseId = rs.getInt("House_ID");
            APT_UNIT = rs.getString("APT_UNIT");
            APT_NUM = rs.getString("APT_NUM");
        } 
        else 
        {
            out.println("<script>");
            out.println("alert('세대 정보를 불러올 수 없습니다.');");
            out.println("history.back();");
            out.println("</script>");
            return;
        }
        rs.close();
        pstmt.close();

        // 오늘 배출량
        sql = "SELECT IFNULL(SUM(Amount),0) AS TodaySum FROM WasteRecord WHERE House_ID = ? AND Record_DATE = CURDATE()";
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
        rs = pstmt.executeQuery();
        if (rs.next()) todayAmount = rs.getInt("TodaySum");
        rs.close();
        pstmt.close();

        // 이번달 배출량
        sql = "SELECT IFNULL(SUM(Amount),0) AS MonthSum FROM WasteRecord WHERE House_ID = ? AND DATE_FORMAT(Record_DATE, '%Y-%m') = DATE_FORMAT(CURDATE(), '%Y-%m')";
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
        rs = pstmt.executeQuery();
        if (rs.next()) monthAmount = rs.getInt("MonthSum");
        rs.close();
        pstmt.close();

        // 최신 청구 정보
        sql = "SELECT Period, Total_Bill, Status, DATE_FORMAT(ADDDATE(CONCAT(Period, '-01'), INTERVAL 23 DAY), '%Y-%m-%d') AS DueDate "
            + "FROM Billing WHERE House_ID = ? ORDER BY Period DESC LIMIT 1";
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
        rs = pstmt.executeQuery();

        if (rs.next()) 
        {
            period = rs.getString("Period");
            totalBill = rs.getInt("Total_Bill");
            status = rs.getString("Status");
            dueDate = rs.getString("DueDate");
        }

        // 최근 7일 배출량
        sql = "SELECT DATE_FORMAT(Record_DATE, '%m-%d') AS d, SUM(Amount) AS total "
            + "FROM WasteRecord WHERE House_ID = ? AND Record_DATE >= DATE_SUB(CURDATE(), INTERVAL 6 DAY) "
            + "GROUP BY d ORDER BY d";
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
        rs = pstmt.executeQuery();
        while (rs.next()) 
        {
            weekLabels.add(rs.getString("d"));
            weekData.add(rs.getInt("total"));
        }
        rs.close();
        pstmt.close();

        // 최근 6개월 납부내역
        sql = "SELECT Period, Total_Bill FROM Billing WHERE House_ID = ? ORDER BY Period DESC LIMIT 6";
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
        rs = pstmt.executeQuery();
        while (rs.next()) 
        {
            monthLabels.add(rs.getString("Period"));
            monthData.add(rs.getInt("Total_Bill"));
        }
        rs.close();
        pstmt.close();

        if (status != null && status.equals("PAID")) 
        {
            paidText = "납부완료";
        } 
        else 
        {
            paidText = "미납";
        }

        if (period != null && period.length() >= 7) 
        {
            periodText = period.substring(5);
        }
        else 
        {
            periodText = "-";
        }
    } 
    catch (Exception e) 
    {
        System.out.println("smart-user DB error: " + e);
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
            System.out.println("smart-user close error: " + e);
        }
    }

    LocalDate today = LocalDate.now();
    String yearText = today.format(DateTimeFormatter.ofPattern("yyyy년"));
    String dayText = today.format(DateTimeFormatter.ofPattern("MM월 dd일(E)"));

    // 차트 전달용 json
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
	<title>Smart Clean : 상세보기</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/detailPage.css">
</head>
<body>

	<div class="wrap">
		<div>
			<h1>Smart Clean System</h1>	
			<div class="logout">
				<a href="register-changeInfo-user.jsp">비밀번호 변경</a>
			</div>
			<div class="logout">
				<a href="index.jsp">Logout</a>
			</div>
		</div>
		<div class="inner-wrap">
			<header>
				<h2>폴리폴리 네이쳐빌리지 아파트</h2>
				<p><%= APT_UNIT %>동 <%= APT_NUM %>호</p>
			</header>

			<main>

				<div class="floor00">
					
					<article class="today-date">
						<h4><%= yearText %></h4>
						<h3><%= dayText %></h3>
					</article>

					<article class="today-total">
						<h4>오늘의 폐기량</h4>
						<h3><%= todayAmount %>g</h3>
					</article>

					<article class="today-bill">
						<h4>이번달 누적 청구액</h4>
						<h3><%= totalBill %>원</h3>
					</article>

				</div>
				
				<div class="floor01">
					
					<article class="month-bill">
						<p class="paid-check"><%= paidText %></p>
						<h4><%= periodText %>월 청구금액</h4>
						<h3 class="month-pay"><%= totalBill %>원</h3>
						<div class="paid-lim">
							<p>납부 마감일</p>
							<p><%= dueDate %></p>
						</div>
						<button>
							<a href="paidCheck-user.jsp">
								우리집 납부내역 확인
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
							<h4>현재 누적 폐기량</h4>
							<h3><%= monthAmount %>g</h3>
						</div>

						<div class="billing">
							<h4>우리집은 음식물 폐기량</h4>
							<h3>상위 13%</h3>
							<p>환경 보호에 앞장서고 있어요!</p>
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
						관리사무소에서 알립니다!
					</a>
					<a href="#" class="ask">
						문의하기
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

	<!-- Chart.js -->
	<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

	<!-- JSP → JS 데이터 전달 -->
	<script>
		// 서버에서 받아온 데이터
		const weekLabels = <%= weekLabelsJson %>;
		const weekData = <%= weekDataJson %>;
		const monthLabels = <%= monthLabelsJson %>;
		const monthData = <%= monthDataJson %>;

		console.log("JSP 전달 데이터 확인");
		console.log("weekLabels =", weekLabels);
		console.log("weekData =", weekData);
		console.log("monthLabels =", monthLabels);
		console.log("monthData =", monthData);
	</script>

	<!-- 외부 JS -->
	<script src="./js/detailChart.js?<%=System.currentTimeMillis()%>"></script>
</body>
</html>