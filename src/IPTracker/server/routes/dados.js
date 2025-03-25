// backend/routes/dados.js
const express = require('express');
const router = express.Router();
const listDados = require('../db/dados');

router.get('/', async (req, res) => {
    try {
        const dados = await listDados();
        res.json({
            success: true,
            data: dados
        });
    } catch (error) {
        console.error('Erro ao obter dados:', error);
        res.status(500).json({
            success: false,
            message: 'Erro interno do servidor'
        });
    }
});

module.exports = router;
