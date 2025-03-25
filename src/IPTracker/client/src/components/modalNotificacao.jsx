import React, { useState } from 'react';
import alerta from '../assets/alerta.png'


const ModalNotificacao = ({ isNotificationOpen, onClose, name }) => {

    const [isContentOpen, setIsContentOpen] = useState(false);

    const openContent = () => {
        setIsContentOpen(true);
    }

    const closeContent = () => {
        setIsContentOpen(false);
    }

    if (!isNotificationOpen) return null;

    return (
        <div>
            <div className="modal-notificacoes">
                <div className="conteudo-modal-notificacoes">
                    <h2>Notificações</h2>
                    <button className='alerta-notificacoes' onClick={openContent}>
                        <img src={alerta} alt='alerta de novas notificações'/>
                        <p>Alerta de necessidade de nova manutenção</p>
                    </button>

                    {isContentOpen && (
                        <div className='conteudo-notificacao'>
                            <h3>Manutenção necessária</h3>
                            <p>O dispositivo <span>{name}</span> alcançou seu tempo limite de funcionamento e necessita de <span>troca de óleo</span></p>
                            <button className='close-notification' onClick={closeContent}>Fechar</button>
                        </div>
                    )}

                    <button className='close-notification' onClick={onClose}>Fechar</button>
                </div>
            </div>
        </div>
    );
}

export default ModalNotificacao;