const yearSelect = document.getElementById('yearSelect');
const tbody = document.querySelector('.monthly-waste tbody');
const tfoot = document.querySelector('.monthly-waste tfoot .total-row');

const data = {
    "2024": [
        { month: "1월", dailyAvg: 720, totalWaste: 22320, totalAmount: 150000, status: "완납" },
        { month: "2월", dailyAvg: 710, totalWaste: 20200, totalAmount: 135000, status: "완납" },
        { month: "3월", dailyAvg: 730, totalWaste: 22600, totalAmount: 152000, status: "완납" },
        { month: "4월", dailyAvg: 740, totalWaste: 22200, totalAmount: 150000, status: "완납" },
        { month: "5월", dailyAvg: 750, totalWaste: 23250, totalAmount: 157500, status: "완납" },
        { month: "6월", dailyAvg: 760, totalWaste: 22800, totalAmount: 154000, status: "완납" },
        { month: "7월", dailyAvg: 745, totalWaste: 23000, totalAmount: 156000, status: "완납" },
        { month: "8월", dailyAvg: 735, totalWaste: 22500, totalAmount: 152500, status: "완납" },
        { month: "9월", dailyAvg: 740, totalWaste: 22200, totalAmount: 150000, status: "완납" },
        { month: "10월", dailyAvg: 750, totalWaste: 23250, totalAmount: 157500, status: "완납" },
        { month: "11월", dailyAvg: 730, totalWaste: 21900, totalAmount: 148500, status: "완납" },
        { month: "12월", dailyAvg: 745, totalWaste: 22350, totalAmount: 151500, status: "완납" }
    ],
    "2025": [
        { month: "1월", dailyAvg: 730, totalWaste: 22500, totalAmount: 152000, status: "완납" },
        { month: "2월", dailyAvg: 720, totalWaste: 21000, totalAmount: 142000, status: "완납" },
        { month: "3월", dailyAvg: 735, totalWaste: 22050, totalAmount: 150500, status: "완납" },
        { month: "4월", dailyAvg: 740, totalWaste: 22300, totalAmount: 151500, status: "완납" },
        { month: "5월", dailyAvg: 750, totalWaste: 23250, totalAmount: 157500, status: "완납" },
        { month: "6월", dailyAvg: 755, totalWaste: 22550, totalAmount: 152500, status: "완납" },
        { month: "7월", dailyAvg: 745, totalWaste: 23000, totalAmount: 155000, status: "완납" },
        { month: "8월", dailyAvg: 735, totalWaste: 22500, totalAmount: 152500, status: "완납" },
        { month: "9월", dailyAvg: 740, totalWaste: 22200, totalAmount: 150000, status: "완납" },
        { month: "10월", dailyAvg: 750, totalWaste: 23250, totalAmount: 157500, status: "미납" },
        { month: "11월", dailyAvg: 0, totalWaste: 0, totalAmount: 0, status: "미납" },
        { month: "12월", dailyAvg: 0, totalWaste: 0, totalAmount: 0, status: "미납" }
    ]
};

function renderTable(year) {
    tbody.innerHTML = "";
    let sumDaily = 0;
    let sumWaste = 0;
    let sumAmount = 0;

    data[year].forEach(item => {
        const tr = document.createElement("tr");
        tr.innerHTML = `
            <td>${item.month}</td>
            <td>${item.dailyAvg}</td>
            <td>${item.totalWaste}</td>
            <td>${item.totalAmount.toLocaleString()}</td>
            <td>${item.status}</td>
        `;
        tbody.appendChild(tr);

        sumDaily += item.dailyAvg;
        sumWaste += item.totalWaste;
        sumAmount += item.totalAmount;
    });

    tfoot.children[1].textContent = sumDaily;
    tfoot.children[2].textContent = sumWaste;
    tfoot.children[3].textContent = sumAmount.toLocaleString();
}

renderTable(yearSelect.value);

yearSelect.addEventListener("change", () => {
    renderTable(yearSelect.value);
});