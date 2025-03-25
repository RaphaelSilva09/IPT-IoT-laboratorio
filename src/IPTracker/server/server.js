const express = require("express");
const bodyParser = require('body-parser'); 
const routes = require('./routes');
const pool = require('./db/database')
const createTables = require('./db/create')

const app = express();

app.use(bodyParser.json());
app.use('/', routes)

// conecta o banco de dados
pool
createTables()

app.use(express.json());

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Servidor aberto na porta ${PORT}`);
});
