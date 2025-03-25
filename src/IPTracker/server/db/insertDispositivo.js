const db = require('./database')

async function insertDispositivo(metrica, limite_metrica, nome) {
    await db.connect()

    const queryDispositivo = `INSERT INTO dispositivo (metrica, limite_metrica, nome) VALUES ($1, $2, $3)`

    await db.query(queryDispositivo, [metrica, limite_metrica, nome])

    console.log('insert de dispositivo concluido!')

}

module.exports = insertDispositivo