import React, { useState } from "react";

const ModalDispositivo = ({ isOpen, onClose }) => {
  const [name, setName] = useState("");
  const [isVibracao, setIsVibracao] = useState(false);
  const [isDistancia, setIsDistancia] = useState(false);
  const [isTempoMax, setIsTempoMax] = useState(false);
  const [isDistMax, setIsDistMax] = useState(false);
  const [tempMax, setTempMax] = useState("10");
  const [distMax, setDistMax] = useState("10");

  const selecionaVibracao = () => {
    setIsVibracao(!isVibracao);
    // Desmarcar distância se vibração for selecionada
    if (!isVibracao) {
      setIsDistancia(false);
    }
  };

  const selecionaDistancia = () => {
    setIsDistancia(!isDistancia);
    // Desmarcar vibração se distância for selecionada
    if (!isDistancia) {
      setIsVibracao(false);
    }
  };

  const selecionaTempoMax = () => {
    setIsTempoMax(!isTempoMax);
    // Desmarcar distância máxima se tempo máximo for selecionado
    if (!isTempoMax) {
      setIsDistMax(false);
    }
  };

  const selecionaDistMax = () => {
    setIsDistMax(!isDistMax);
    // Desmarcar tempo máximo se distância máxima for selecionada
    if (!isDistMax) {
      setIsTempoMax(false);
    }
  };

  const backendFetch = async () => {
    try {
      // Determinar a métrica correta para envio
      const metrica = isVibracao ? "vibracao" : "distancia";
      const limite_metrica = isVibracao ? tempMax : distMax;

      const response = await fetch("/novoDispositivo", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          metrica,
          limite_metrica,
          nome: name,
        }),
      });

      if (!response.ok) {
        throw new Error("Erro na requisição");
      }

      alert("Dispositivo cadastrado com sucesso");
      onClose(); // Fechar o modal após cadastro bem-sucedido
    } catch (error) {
      console.error("Erro ao cadastrar dispositivo:", error);
      alert("Erro ao cadastrar dispositivo");
    }
  };

  const verificacao = () => {
    if (name === "") {
      alert("Por favor, dê um nome ao dispositivo");
      return;
    }

    if (isDistancia && isVibracao) {
      alert("Por favor, selecione apenas uma métrica de funcionamento");
      return;
    }

    if (!isDistancia && !isVibracao) {
      alert("Por favor, selecione ao menos uma métrica de funcionamento");
      return;
    }

    if (isDistMax && isTempoMax) {
      alert("Por favor, selecione apenas uma métrica de controle");
      return;
    }

    const backendFetch = async () => {
        try {
            const response = await fetch('/novoDispositivo', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    metrica: isVibracao ? 'vibracao' : 'distancia',
                    limite_metrica: isVibracao ? tempMax : distMax,
                    nome: name
                })
            })
            if (!response.ok) {
                throw new Error('Erro na requisição')
            }
            alert('Dispositivo cadastrado com sucesso')
        } catch {
            alert('Erro ao cadastrar dispositivo')
        }
    }
    const verificacao = () => {
        if (name === '') {
            alert('Por favor, dê um nome ao dispositivo')
        } else if (isDistancia && isVibracao) {
            alert('Por favor, selecione apenas uma métrica de funcionamento')
        } else if (!isDistancia && !isVibracao) {
            alert('Por favor, selecione ao menos uma métrica de funcionamento')
        } else if (isDistMax && isTempoMax) {
            alert('Por favor, selecione apenas uma métrica de controle')
        } else if (!isDistMax && !isTempoMax) {
            alert('Por favor, selecione ao menos uma métrica de controle')
        } else {
            onClose();
        }
    }

    // Se todas as verificações passarem, chama o método para enviar ao backend
    backendFetch();
  };

  if (!isOpen) return null;

  return (
    <div className="modal-dispositivo">
      <div className="conteudo-modal-dispositivo">
        <h2>Cadastrar novo dispositivo</h2>
        <label>Nome:</label>
        <input
          type="text"
          value={name}
          onChange={(e) => setName(e.target.value)}
        />

                <label>Métrica de atividade:</label>
                <div className="metricas">
                    <input type="checkbox" id="vibracao" name="vibracao" onChange={selecionaVibracao} />
                    <label htmlFor="vibracao">Vibração</label>
                    <input type="checkbox" id="distancia" name="distancia" onChange={selecionaDistancia} />
                    <label htmlFor="distancia">Distância</label>
                </div>

                <label>Métricas de controle:</label>
                <div className="select-tempo">
                    <label htmlFor="intervalo">Intervalo de vibração: </label>
                    <select id="intervalo" name="intervalo">
                        <option>10</option>
                        <option>30</option>
                        <option>60</option>
                        <option>120</option>
                    </select>
                </div>

        <div className="select-tempo">
          <input
            type="checkbox"
            name="tempoMaximo"
            checked={isTempoMax}
            onChange={selecionaTempoMax}
          />
          <label htmlFor="intervalo">Tempo Máximo: </label>
          <select
            id="intervalo"
            name="intervalo"
            value={tempMax}
            onChange={(e) => setTempMax(e.target.value)}
          >
            <option>10</option>
            <option>30</option>
            <option>60</option>
            <option>120</option>
          </select>
        </div>

        <div className="select-tempo">
          <input
            type="checkbox"
            name="distanciaMaxima"
            checked={isDistMax}
            onChange={selecionaDistMax}
          />
          <label htmlFor="dist-maxima">Distância máxima: </label>
          <select
            id="dist-maxima"
            name="dist-maxima"
            value={distMax}
            onChange={(e) => setDistMax(e.target.value)}
          >
            <option>10</option>
            <option>30</option>
            <option>60</option>
            <option>120</option>
          </select>
        </div>

                <button onClick={verificacao}>Ok</button>
            </div>
        </div>
    );
};

export default ModalDispositivo;
