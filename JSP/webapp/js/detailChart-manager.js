document.addEventListener("DOMContentLoaded", function() {

    // 주간 폐기량
    const ctxWeek = document.getElementById("weekChart");
    if (ctxWeek && weekLabels && weekData) 
    {
        new Chart(ctxWeek, {
            type: 'line',
            data: {
                labels: weekLabels,
                datasets: [{
                    label: '일별 폐기량 (g)',
                    data: weekData,
                    borderWidth: 2,
                    borderColor: '#007bff',
                    backgroundColor: 'rgba(0, 123, 255, 0.2)',
                    tension: 0.3,
                    fill: true
                }]
            },
            options: {
                responsive: true,
                plugins: { legend: { display: false } },
                scales: {
                    y: { beginAtZero: true }
                }
            }
        });
    }

    // 월별 청구금액
    const ctxMonth = document.getElementById("monthChart");
    if (ctxMonth && monthLabels && monthData) 
    {
        new Chart(ctxMonth, {
            type: 'bar',
            data: {
                labels: monthLabels,
                datasets: [{
                    label: '월별 총 청구액 (₩)',
                    data: monthData,
                    backgroundColor: 'rgba(40, 167, 69, 0.4)',
                    borderColor: '#28a745',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                plugins: { legend: { display: false } },
                scales: {
                    y: { beginAtZero: true }
                }
            }
        });
    }
});