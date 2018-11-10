$(document).ready(function () {
  Highcharts.chart('container', {
      chart: {
        type: 'gauge',
        plotBackgroundColor: null,
        plotBackgroundImage: null,
        plotBorderWidth: 0,
        plotShadow: false
      },
      title: {
        text: 'Nykyinen lämpötila'
      },
      credits: {
        enabled: false
      },
      pane: {
        startAngle: -150,
        endAngle: 150,
        background: [{
          backgroundColor: {
            linearGradient: {
              x1: 0,
              y1: 0,
              x2: 0,
              y2: 1
            },
            stops: [
              [0, '#FFF'],
              [1, '#333']
            ]
          },
          borderWidth: 0,
          outerRadius: '109%'
        }, {
          backgroundColor: {
            linearGradient: {
              x1: 0,
              y1: 0,
              x2: 0,
              y2: 1
            },
            stops: [
              [0, '#333'],
              [1, '#FFF']
            ]
          },
          borderWidth: 1,
          outerRadius: '107%'
        }, {
          // default background
        }, {
          backgroundColor: '#DDD',
          borderWidth: 0,
          outerRadius: '105%',
          innerRadius: '103%'
        }]
      },

      // the value axis
      yAxis: {
        min: 0,
        max: 1024,

        minorTickInterval: 'auto',
        minorTickWidth: 1,
        minorTickLength: 10,
        minorTickPosition: 'inside',
        minorTickColor: '#666',

        tickPixelInterval: 30,
        tickWidth: 1,
        tickPosition: 'side',
        tickLength: 10,
        tickColor: '#666',
        labels: {
          step: 2,
          rotation: 'auto'
        },
        title: {
          text: ''
        },
        plotBands: [{
            from: 0,
            to: 100,
            color: '#ffdd42'
          }, {
            from: 100,
            to: 300,
            color: '#FFA500'
          }, {
            from: 300,
            to: 500,
            color: '#FF8C00'
          }, {
            from: 500,
            to: 800,
            color: '#df7f00'
          },
          {
            from: 800,
            to: 1024,
            color: '#FF4500'
          }
        ]
      },

      series: [{
        name: 'Lämpotila',
        data: [0],
        tooltip: {
          valueSuffix: '°C'
        }
      }]

    },

    // Add some life
    function (chart) {
      if (!chart.renderer.forExport) {
        var point = chart.series[0].points[0];
      
        (function getTempData() {
          $.ajax({
            url: '/temp', 
            success: function(data) {
              if(data && data.current_temp) {
                point.update(data.current_temp);
              } 
            },
            complete: function() {
              setTimeout(getTempData, 1000);
            }
          });
        })();
      }
    });
});