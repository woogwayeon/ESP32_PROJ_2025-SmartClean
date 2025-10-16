<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ page import="java.sql.*, java.io.*, java.util.*, jakarta.servlet.*, jakarta.servlet.annotation.*, jakarta.servlet.http.*" %>
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

    int houseId = 0;
    String APT_UNIT = "";
    String APT_NUM = "";

    try 
    {
        Class.forName("com.mysql.cj.jdbc.Driver");
        conn = DriverManager.getConnection(dbURL, dbID, dbPW);

        String sql = "SELECT House_ID, APT_UNIT, APT_NUM FROM House WHERE User_ID = ?";
       
        pstmt = conn.prepareStatement(sql);
        pstmt.setString(1, userId);
       
        rs = pstmt.executeQuery();

        if (rs.next()) 
        {
            houseId = rs.getInt("House_ID");
            APT_UNIT = rs.getString("APT_UNIT");
            APT_NUM = rs.getString("APT_NUM");
        }
        
        rs.close();
        pstmt.close();

        sql = "SELECT Period, Total_Bill, Status FROM Billing WHERE House_ID = ? ORDER BY Period ASC LIMIT 6";
        
        pstmt = conn.prepareStatement(sql);
        pstmt.setInt(1, houseId);
       
        rs = pstmt.executeQuery();

        List<Map<String, Object>> billList = new ArrayList<Map<String, Object>>();
        
        while (rs.next()) 
        {
            Map<String, Object> map = new HashMap<String, Object>();
           
            map.put("Period", rs.getString("Period"));
            map.put("Total_Bill", rs.getInt("Total_Bill"));
            map.put("Status", rs.getString("Status"));
           
            billList.add(map);
        }
        
        rs.close();
        pstmt.close();
%>
<!DOCTYPE html>
<html lang="ko">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Smart Clean : 우리집 납부내역 확인</title>
	<link rel="stylesheet" href="./style/default.css">
	<link rel="stylesheet" href="./style/detailPage.css">
	<link rel="stylesheet" href="./style/paidDetail.css">
</head>
<body>

	<div class="wrap">
		<div>
			<h1>Smart Clean System : Paid</h1>	
			<div class="logout">
				<a href="LogoutServlet">Logout</a>
			</div>
		</div>
		<div class="inner-wrap">
			<header>
				<h2>우리집 납부내역 확인하기</h2>
				<p><%= APT_UNIT %>동 <%= APT_NUM %>호</p>
			</header>

			<main>
				<div class="floor01">
					<article class="month-bill">
						    <table class="monthly-waste">
						        <thead>
						            <tr>
						                <th>월</th>
						                <th>총 배출량(g)</th>
						                <th>총 금액(₩)</th>
						                <th>감축률(%)</th>
						                <th>납부 상태</th>
						            </tr>
						        </thead>
						        <tbody>
						        <%
						            int prevTotal = 0;
						           
						        	for (Map<String, Object> row : billList) 
						            {
						                String period = (String) row.get("Period");
						                int totalBill = (int) row.get("Total_Bill");
						                String status = (String) row.get("Status");

						                PreparedStatement pstmt2 = null;
						                ResultSet rs2 = null;
						                
						                String wasteSql = "SELECT IFNULL(SUM(Amount),0) AS TotalAmount "
						                                + "FROM WasteRecord WHERE House_ID = ? AND DATE_FORMAT(Record_DATE, '%Y-%m') = ?";
						                
						                pstmt2 = conn.prepareStatement(wasteSql);
						                pstmt2.setInt(1, houseId);
						                pstmt2.setString(2, period);
						               
						                rs2 = pstmt2.executeQuery();

						                int totalAmount = 0;
						               
						                if (rs2.next()) 
						                {
						                    totalAmount = rs2.getInt("TotalAmount");
						                }
						                
						                rs2.close();
						                pstmt2.close();

						                String reductionText = "";
						                
						                if (prevTotal > 0) 
						                {
						                    double reductionRate = ((double)(prevTotal - totalAmount) / prevTotal) * 100.0;
						                    
						                    if (reductionRate > 0) 
						                    {
						                        reductionText = String.format("-%.1f%%", reductionRate);
						                    } 
						                    else if (reductionRate < 0) 
						                    {
						                        reductionText = String.format("+%.1f%%", Math.abs(reductionRate));
						                    } 
						                    else 
						                    {
						                        reductionText = "변동없음";
						                    }
						                } 
						                else 
						                {
						                    reductionText = "기준";
						                }
						               
						                prevTotal = totalAmount;

						                String statusText = "";
						               
						                if (status != null && status.equals("PAID")) 
						                {
						                    statusText = "완납";
						                } 
						                else 
						                {
						                    statusText = "미납";
						                }
						        %>
						            <tr>
						                <td><%= period.substring(5) %>월</td>
						                <td><%= totalAmount %></td>
						                <td><%= totalBill %></td>
						                <td><%= reductionText %></td>
						                <td><%= statusText %></td>
						            </tr>
						        <%
						            } 
						        %>
						        </tbody>
						    </table>
					</article>
				</div>
				
				<div class="floor03">
					<a href="#" class="noti">
						납부내역 문의하기
					</a>
					<a href="smart-user.jsp" class="ask">
						뒤로 돌아가기
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
</body>
</html>
<%
    } 
    catch (Exception e) 
    {
        System.out.println("paidCheck-user 감축률 error: " + e);
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
            System.out.println("paidCheck-user close error: " + e);
        }
    }
%>