import React, { Component } from "react";
import LineChart from "./grafico";

class Dashboard extends Component {
  constructor(props) {
    super(props);
    this.state = {
      dispositivo: "",
      metrica: "",
      periodo: "",
      data: [],
      loading: true,
      error: null,
    };

    this.selecionaDispositivo = this.selecionaDispositivo.bind(this);
    this.selecionaMetrica = this.selecionaMetrica.bind(this);
    this.selecionaPeriodo = this.selecionaPeriodo.bind(this);
  }

  componentDidMount() {
    this.fetchBackendData();
  }

  async fetchBackendData() {
    this.setState({ data: null });
    try {
      const response = await fetch("/dispositivos");
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      const result = await response.json();
      this.setState({
        data: result.data,
        loading: false,
      });
    } catch (error) {
      console.error("Error fetching data:", error);
      this.setState({
        error: error.message,
        loading: false,
      });
    }
  }

  selecionaDispositivo(e) {
    const valorSelecionado = e.target.value;
    this.setState({
      dispositivo: valorSelecionado === "Dispositivo" ? "" : valorSelecionado,
      metrica: "",
      periodo: ""
    });
  }

  selecionaMetrica(e) {
    this.setState({ metrica: e.target.value });
  }

  selecionaPeriodo(e) {
    this.setState({ periodo: e.target.value });
  }

  render() {
    const { data, loading, error } = this.state;

    if (loading) return <div>Carregando...</div>;
    if (error) return <div>Erro: {error}</div>;

    return (
      <div className="dispositivo">
        <div className="config-dispositivo">
          <select
            className="dropDown"
            onChange={(e) => {
              this.selecionaDispositivo(e);
            }}
            value={this.state.dispositivo}
          >
            <option value="">Dispositivo</option>
            {data && data.length > 0 ? (
              data.map((dispositivo) => (
                <option
                  key={dispositivo.id}
                  value={dispositivo.nome || dispositivo.name}
                >
                  {dispositivo.nome || dispositivo.name}
                </option>
              ))
            ) : (
              <option>Nenhum dispositivo disponível</option>
            )}
          </select>

          <select
            className="dropDown"
            onChange={(event) => {
              this.selecionaMetrica(event);
            }}
            value={this.state.metrica}
          > <option value="">N/A dispositivo</option>
          {data && data.length > 0 ? (
              data
                .filter((dispositivo) => dispositivo.nome === this.state.dispositivo)
                .map((dispositivo) => (
                  <option key={dispositivo.id} value={dispositivo.metrica}>
                    {dispositivo.metrica}
                  </option>
                ))
            ) : (
              <option>Métrica indisponível</option>
            )}
          </select>

          <select
            className="dropDown"
            onChange={this.selecionaPeriodo}
            value={this.state.periodo}
          >
            <option value="">Período</option>
            <option value="3">3 meses</option>
            <option value="6">6 meses</option>
            <option value="12">12 meses</option>
          </select>
        </div>
        <div className="dashboard">
          <LineChart
            Dispositivo={this.state.dispositivo}
            Metrica={this.state.metrica}
            Periodo={this.state.periodo}
          />
        </div>
      </div>
    );
  }
}

export default Dashboard;
