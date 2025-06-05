document.addEventListener('DOMContentLoaded', () => {
  fetch('modcv.json')
    .then(res => {
      if (!res.ok) throw new Error('Failed to load data');
      return res.json();
    })
    .then(data => {
      const container = document.getElementById('data-container');

      // Group by Variable Category for clarity
      const grouped = {};
      data.forEach(item => {
        const category = item.Variable_category;
        if (!grouped[category]) grouped[category] = [];
        grouped[category].push(item);
      });

      for (const [category, items] of Object.entries(grouped)) {
        const section = document.createElement('section');
        section.classList.add('data-section');

        const title = document.createElement('h2');
        title.textContent = category;
        section.appendChild(title);

        items.forEach(item => {
          const div = document.createElement('div');
          div.className = 'data-item';
          div.innerHTML = `
            <strong>${item.Variable_name}</strong><br>
            Value: ${item.Value.toLocaleString()} ${item.Units}
            <hr />
          `;
          section.appendChild(div);
        });

        container.appendChild(section);
      }

      // ✅ Create Total Income by Industry Bar Chart (2023)
      const incomeData = data.filter(item =>
        item.Variable_name === "Total income" && item.Year === 2023
      );

      if (incomeData.length > 0) {
        const labels = incomeData.map(item => item.Industry_name_NZSIOC);
        const values = incomeData.map(item => Number(item.Value));

        const ctx = document.getElementById('incomeChart').getContext('2d');
        new Chart(ctx, {
          type: 'bar',
          data: {
            labels,
            datasets: [{
              label: 'Total Income (millions)',
              data: values,
              backgroundColor: 'rgba(59, 130, 246, 0.6)',
              borderColor: 'rgba(59, 130, 246, 1)',
              borderWidth: 1
            }]
          },
          options: {
            responsive: true,
            plugins: {
              title: {
                display: true,
                text: 'Total Income by Industry (2023)',
                color: '#fff',
                font: { size: 20 }
              },
              legend: {
                labels: { color: '#fff' }
              }
            },
            scales: {
              x: {
                ticks: { color: '#fff' }
              },
              y: {
                beginAtZero: true,
                ticks: { color: '#fff' }
              }
            }
          }
        });
      }

    })
    .catch(err => {
      console.error(err);
      document.getElementById('data-container').innerHTML = '<p>Error loading data.</p>';
    });
  document.addEventListener('DOMContentLoaded', () => {
  const hamburger = document.getElementById('hamburgerBtn');
  const mobileMenu = document.getElementById('mobileMenu');
  const overlay = document.getElementById('overlay');

  if (hamburger && mobileMenu && overlay) {
    hamburger.addEventListener('click', () => {
      mobileMenu.classList.toggle('show');
      overlay.classList.toggle('active');
      document.body.classList.toggle('menu-open');
    });

    overlay.addEventListener('click', () => {
      mobileMenu.classList.remove('show');
      overlay.classList.remove('active');
      document.body.classList.remove('menu-open');
    });

    document.addEventListener('click', (e) => {
      if (
        !mobileMenu.contains(e.target) &&
        !hamburger.contains(e.target) &&
        !overlay.contains(e.target)
      ) {
        mobileMenu.classList.remove('show');
        overlay.classList.remove('active');
        document.body.classList.remove('menu-open');
      }
    });
  }
});
});


