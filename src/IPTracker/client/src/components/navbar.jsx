import React, { Component } from 'react';
import logo from '../assets/logo.svg';
import notificacao from '../assets/notificacao.svg';
import ModalDispositivo from './modalDispositivo';
import ModalNotificacao from './modalNotificacao';

class Navbar extends Component {
    constructor(props) {
        super(props);
        this.state = {
            name: '',
            isModalDispOpen: false,
            isModalNotificationOpen: false,
        };

        this.openDispModal = this.openDispModal.bind(this);
        this.closeDispModal = this.closeDispModal.bind(this);
        this.openNotificationModal = this.openNotificationModal.bind(this);
        this.closeNotificationModal = this.closeNotificationModal.bind(this);
        this.changeName = this.changeName.bind(this);
    }

    openDispModal() {
        this.setState({ name: '' });
        this.setState({ isModalDispOpen: true });
    }

    closeDispModal() {
        this.setState({ isModalDispOpen: false });
    }

    openNotificationModal() {
        this.setState({ isModalNotificationOpen: true });
    }

    closeNotificationModal() {
        this.setState({ isModalNotificationOpen: false });
    }

    changeName(e) {
        let nomeDigitado = e.target.value;
        this.setState({ name: nomeDigitado });
    }

    render() {
        return (
            <div>
                <nav className="navbar">
                    <div className="logo">
                        <img src={logo} alt="Logo" />
                    </div>

                    <div className="ferramentasNav">
                        <button className="novoDispositivo" onClick={this.openDispModal}>
                            <span>+</span> Novo Dispositivo
                        </button>

                        <button className="notificacao" onClick={this.openNotificationModal}>
                            <img src={notificacao} alt="Notificação" />
                        </button>
                    </div>
                </nav>

                <ModalDispositivo
                    isOpen={this.state.isModalDispOpen}
                    onClose={this.closeDispModal}
                    name={this.state.name}
                    onNameChange={this.changeName}
                />

                <ModalNotificacao
                    isNotificationOpen={this.state.isModalNotificationOpen}
                    onClose={this.closeNotificationModal}  
                    name={this.state.name} 
                />
            </div>
        );
    }
}

export default Navbar;
