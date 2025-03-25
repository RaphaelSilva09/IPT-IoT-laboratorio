import React, { Component } from 'react';

class Historico extends Component {
    constructor(props) {
        super(props);
        this.state = {
            ultimaRevisao: '',
            trocaOleo: '',
            ultimaManutencao: ''
        }
    }

    render() {
        const { data } = this.props;

        return (
            <div className="historico">
                <div className="dados-historico">
                    <h2>Histórico</h2>
                    <h3>Última revisão: {this.ultimaRevisao ? `${this.ultimaRevisao}` : "00/00/00"}</h3>
                    <h3>Última troca de óleo: {this.trocaOleo ? `${this.trocaOleo}` : "00/00/00"}</h3>
                    <h3>Última manutenção: {this.ultimaRevisao ? `${this.ultimaManutencao}` : "00/00/00"}</h3>
                </div>
                <div className="tempo-funcionamento">
                    <h3>Tempo de funcionamento</h3>
                    <h2>00:00:00</h2>
                </div>
            </div>
        )
    }
}

export default Historico;