const router = require('express').Router();

require('../db/database');

const dispositivos = require('./dispositivos');
const novoDispositivo = require('./novoDispositivo')
const dados = require('./dados')

// Redireciona as requisições na rota /portfolio para o arquivo que controla essas rotas 
router.use('/dispositivos', dispositivos);
router.use('/novoDispositivo', novoDispositivo);
router.use('/dados', dados);

module.exports = router;
