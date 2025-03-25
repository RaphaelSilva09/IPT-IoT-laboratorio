import React from 'react';
import { Line } from 'react-chartjs-2';
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend } from 'chart.js';

ChartJS.register(
   CategoryScale,
   LinearScale,
   PointElement,
   LineElement,
   Title,
   Tooltip,
   Legend
);

const LineChart = ({ Dispositivo, Metrica, Periodo }) => {
   const labels = [['Janeiro', 'Fevereiro', 'Março'], ['Janeiro', 'Fevereiro', 'Março', 'Abril', 'Maio', 'Junho'], ['Janeiro', 'Fevereiro', 'Março', 'Abril', 'Maio', 'Junho', 'Julho', 'Agosto', 'Setembro', 'Outubro', 'Novembro', 'Dezembro']];
   let label = [];
   let dataset = [];

   const datasets = [
       [
           { label: 'Temperatura', data: [65, 89, 68], fill: false, borderColor: '#165a73', tension: 0.5 },
           { label: 'Vibração', data: [15, 20, 18], fill: false, borderColor: '#ff5733', tension: 0.5 }
       ],
       [
           { label: 'Temperatura', data: [65, 89, 68, 66, 70, 90], fill: false, borderColor: '#165a73', tension: 0.5 },
           { label: 'Vibração', data: [12, 18, 21, 19, 16, 20], fill: false, borderColor: '#ff5733', tension: 0.5 }
       ],
       [
           { label: 'Temperatura', data: [65, 89, 68, 66, 70, 90, 65, 89, 68, 66, 70, 90], fill: false, borderColor: '#165a73', tension: 0.5 },
           { label: 'Vibração', data: [14, 19, 21, 23, 18, 20, 15, 22, 19, 17, 21, 20], fill: false, borderColor: '#ff5733', tension: 0.5 }
       ]
   ];

   if (Periodo === '3') {
       label = labels[0];
       dataset = datasets[0];
   } else if (Periodo === '6') {
       label = labels[1];
       dataset = datasets[1];
   } else if (Periodo === '12') {
       label = labels[2];
       dataset = datasets[2];
   }

   const selectedDataset = dataset.filter(ds => 
       ds.label.toLowerCase() === Metrica.toLowerCase()
   );

   const data = {
       labels: label,
       datasets: selectedDataset
   };

   const options = {
       responsive: true,
       plugins: {
           legend: {
               position: 'top',
           },
           tooltip: {
               callbacks: {
                   label: (tooltipItem) => `Valor: ${tooltipItem.raw}`,
               },
           },
       },
       scales: {
           x: {
               title: {
                   display: true,
                   text: 'Meses',
               },
           },
           y: {
               title: {
                   display: true,
                   text: Metrica === 'temperatura' ? '°C' : 'Unidade de Vibração',
               },
           },
       },
   };

   return (
       <div className='grafico'>
           {Dispositivo === '' ? 
               <h2>Selecione o dispositivo</h2> :
               <>
                   <h2>{`Gráfico de ${Metrica.charAt(0).toUpperCase() + Metrica.slice(1)}`}</h2>
                   <Line data={data} options={options} />
               </>
           }
       </div>
   );
};

export default LineChart;