// week chart
const weekCtx = document.getElementById('weekChart').getContext('2d');
const weekConfig = {
    type: 'bar',
    data: {
        labels: ['9/30(월)', '10/1(화)', '10/2(수)', '10/3(목)', '10/4(금)', '10/5(토)', '10/6(일)'],
        datasets: [{
            label: '쓰레기 배출량(g)',
            data: [30, 40, 35, 50, 45, 20, 30],
            backgroundColor: 'rgba(79, 195, 247, 0.7)'
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false
        // plugins: {
        //     legend: { display: true },
        //     title: { display: true, text: '주간 쓰레기 배출량' }
        // }
    }
};
const weekChart = new Chart(weekCtx, weekConfig);

// month chart
const monthCtx = document.getElementById('monthChart').getContext('2d');
const monthConfig = {
    type: 'line',
    data: {
        labels: ['1월','2월','3월','4월','5월','6월','7월','8월','9월'],
        datasets: [{
            label: '청구금액(원)',
            data: [120, 150, 110, 180, 200, 170, 210, 190, 330],
            borderColor: 'rgba(129, 199, 132, 0.8)',
            backgroundColor: 'rgba(129, 199, 132, 0.3)',
            fill: true,
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        // plugins: {
        //     legend: { display: true },
        //     title: { display: true, text: '월별 청구금액' }
        // }
    }
};
const monthChart = new Chart(monthCtx, monthConfig);