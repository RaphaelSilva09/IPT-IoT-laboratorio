const router = require('express').Router()
const listDispositivos = require('../db/listDispositivos')

router.get('/', async (req, res) => {
    try {
        const dispositivos = await listDispositivos();
        res.json({
            success: true,
            data: dispositivos
        });
    } catch (error) {
        console.error('Erro ao obter dispositivos:', error);
        res.status(500).json({
            success: false,
            message: 'Erro interno do servidor'
        });
    }
});
module.exports = router