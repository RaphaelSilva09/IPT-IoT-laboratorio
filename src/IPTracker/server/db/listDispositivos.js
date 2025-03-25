const db = require('./database')

async function listDispositivos() {
    await db.connect()
    let dispositivos = await db.query(`SELECT * FROM dispositivo;`)
    dispositivos = dispositivos.rows
    console.log('Lista de dispositivos disponíveis:', dispositivos)
    return dispositivos
}

module.exports = listDispositivos