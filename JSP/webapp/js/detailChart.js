document.addEventListener("DOMContentLoaded", function() 
{
    console.log("detailChart.js loaded");

    function hasData(arr) 
    {
        return Array.isArray(arr) && arr.length >= 0;
    }

    function formatMD(date) 
    {
        const m = (date.getMonth() + 1).toString().padStart(2, "0");
        const d = date.getDate().toString().padStart(2, "0");
        return m + "-" + d;
    }

    function recent7DaysLabels() 
    {
        const labels = [];
        for (let i = 6; i >= 0; i--) 
        {
            const d = new Date();
            d.setDate(d.getDate() - i);
            labels.push(formatMD(d));
        }
        return labels;
    }

    function recent6MonthsLabels() 
    {
        const labels = [];
        const today = new Date();
        for (let i = 5; i >= 0; i--) 
        {
            const d = new Date(today.getFullYear(), today.getMonth() - i, 1);
            const y = d.getFullYear();
            const m = (d.getMonth() + 1).toString().padStart(2, "0");
            labels.push(y + "-" + m);
        }
        return labels;
    }

    if (typeof weekLabels === "undefined" || !hasData(weekLabels)) 
    {
        console.log("weekLabels 없음 → 최근 일주일 자동 생성");
        weekLabels = recent7DaysLabels();
    }
    if (typeof weekData === "undefined" || !hasData(weekData)) 
    {
        console.log("weekData 없음 → 0으로 채움");
        weekData = new Array(weekLabels.length).fill(0);
    }
    if (typeof monthLabels === "undefined" || !hasData(monthLabels)) 
    {
        console.log("monthLabels 없음 → 최근 육개월 자동 생성");
        monthLabels = recent6MonthsLabels();
    }
    if (typeof monthData === "undefined" || !hasData(monthData)) 
    {
        console.log("monthData 없음 → 0으로 채움");
        monthData = new Array(monthLabels.length).fill(0);
    }

    const weekCanvas = document.getElementById("weekChart");
    const monthCanvas = document.getElementById("monthChart");

    // 최근 일주일 배출량 그래프
    if (weekCanvas && typeof Chart !== "undefined") 
    {
        const weekCtx = weekCanvas.getContext("2d");

        new Chart(weekCtx, 
        {
            type: "bar",
            data: 
            {
                labels: weekLabels,
                datasets: [
                    {
                        label: "일별 배출량 (g)",
                        data: weekData,
                        backgroundColor: "rgba(54, 162, 235, 0.5)",   // 파란색 배경
                        borderColor: "rgba(54, 162, 235, 1)",        // 파란색 테두리
                        borderWidth: 2,
                        borderRadius: 10,
                        barThickness: 40
                    }
                ]
            },
            options: 
            {
                responsive: true,
                plugins: 
                {
                    title: 
                    {
                        display: true,
                        text: "최근 7일 배출량 변화",
                        font: { size: 16, weight: "bold" },
                        padding: { top: 10, bottom: 20 }
                    },
                    legend: { display: false },
                    tooltip: 
                    {
                        callbacks: 
                        {
                            label: function(context) 
                            {
                                return context.parsed.y + " g";
                            }
                        }
                    }
                },
                scales: 
                {
                    y: 
                    {
                        min: 0,
                        suggestedMax: Math.max(...weekData) * 1.2,
                        title: { display: true, text: "배출량 (g)" },
                        grid: { color: "rgba(210, 210, 210, 0.25)" }
                    },
                    x: 
                    {
                       // title: { display: true, text: "날짜" }
                    }
                }
            }
        });
    }

    // 최근 육개월 납부금액 그래프
    if (monthCanvas && typeof Chart !== "undefined") 
    {
        const monthCtx = monthCanvas.getContext("2d");

        const combined = monthLabels.map(function(l, i) 
        { 
            return { label: l, value: monthData[i] || 0 }; 
        });
        combined.sort(function(a, b) { return a.label.localeCompare(b.label); });
        const sortedLabels = combined.map(function(c) { return c.label; });
        const sortedData = combined.map(function(c) { return c.value; });

        new Chart(monthCtx, 
        {
            type: "line",
            data: 
            {
                labels: sortedLabels,
                datasets: [
                    {
                        label: "월별 납부 금액 (원)",
                        data: sortedData,
                        fill: true,
                        borderColor: "rgba(56, 142, 60, 1)",       // 연두색 라인
                        backgroundColor: "rgba(102, 187, 106, 0.25)", // 연두색 면
                        pointBackgroundColor: "rgba(46, 125, 50, 1)",
                        pointRadius: 6,
                        tension: 0.35,
                        borderWidth: 3
                    }
                ]
            },
            options: 
            {
                responsive: true,
                plugins: 
                {
                    title: 
                    {
                        display: true,
                        text: "최근 6개월 납부 금액 추이",
                        font: { size: 16, weight: "bold" },
                        padding: { top: 10, bottom: 20 }
                    },
                    legend: { display: false },
                    tooltip: 
                    {
                        callbacks: 
                        {
                            label: function(context) 
                            {
                                return context.parsed.y.toLocaleString() + " 원";
                            }
                        }
                    }
                },
                scales: 
                {
                    y: 
                    {
                        min: 0,
                        suggestedMax: Math.max(...sortedData) * 1.2,
                        // title: { display: true, text: "납부금액 (원)" },
                        ticks: 
                        {
                            callback: function(value) 
                            {
                                return value.toLocaleString() + "원";
                            }
                        },
                        grid: { color: "rgba(210, 210, 210, 0.25)" }
                    },
                    x: 
                    {
                       // title: { display: true, text: "기간 (YYYY-MM)" }
                    }
                }
            }
        });
    }
});