# Inteli - Instituto de Tecnologia e Liderança 

<p align="center">
<a href= "https://www.inteli.edu.br/"><img src="./document/assets/inteli.png" alt="Inteli - Instituto de Tecnologia e Liderança" border="0" width=40% height=40%></a>
</p>

<br>

# IPTracker

## IPTrack
## 👨‍🎓 Integrantes: 
- <a href="https://www.linkedin.com/in/ana-beggiato/">Ana Beggiato</a>
- <a href="https://www.linkedin.com/in/eduardo-rizk/">Eduardo Rizk </a>
- <a href="https://www.linkedin.com/in/igor-sampaio-silva/">Igor Sampaio Silva </a> 
- <a href="https://www.linkedin.com/in/larissa-martins-304644245/">Larissa Martins </a> 
- <a href="https://www.linkedin.com/in/pedro-haouli/">Pedro Haouli</a>
- <a href="https://www.linkedin.com/in/raphaelfelipesilva/">Raphael Felipe Silva</a> 
- <a href="https://www.linkedin.com/in/ryan-botelho-gartlan/">Ryan Botelho Gartlan</a>

## 👩‍🏫 Professores:
### Orientador(a) 
- <a href="https://www.linkedin.com/in/marcelo-gonçalves-phd-a550652/">Marcelo Gonçalves</a>
### Instrutores
- <a href="https://www.linkedin.com/in/andregodoichiovato/">André Godoi Chiovato </a>
- <a href="https://www.linkedin.com/in/michele-bazana-de-souza-69b77763/">Michele Bazana De Souza</a> 
- <a href="https://www.linkedin.com/in/geraldo-magela-severino-vasconcelos-22b1b220/">Geraldo Magela Severino Vasconcelos</a> 
- <a href="https://www.linkedin.com/in/francisco-escobar/edit/forms/next-action/after-connect-add-position/">Francisco Escobar</a>
- <a href="https://www.linkedin.com/in/egondaxbacher/">Egon Daxbacher</a> 

## 📜 Descrição

&nbsp;&nbsp;&nbsp;&nbsp;O projeto consiste em um sistema de Internet das Coisas (IoT) que integra hardware de baixo custo e uma plataforma web para o monitoramento de equipamentos de laboratório, como compressores de ar e prensas. O sistema foi desenvolvido com o objetivo de reduzir custos operacionais, prolongar a vida útil dos equipamentos e melhorar a manutenção preventiva, garantindo maior disponibilidade e eficiência.

&nbsp;&nbsp;&nbsp;&nbsp;No hardware, um ESP32 coleta dados de diversos sensores conectados, como o BME280 (temperatura e umidade), o MMA8455X (vibração), e o HC-SR04 (distância). Esses sensores fornecem informações detalhadas sobre o estado dos equipamentos, permitindo a detecção de anomalias ou situações que demandem intervenção. Os dados coletados são enviados via protocolo MQTT para um broker, que então os transmite para um banco de dados.

&nbsp;&nbsp;&nbsp;&nbsp;A plataforma web permite que os usuários personalizarem a configuração do dispositivo IoT de acordo com as necessidades específicas de cada máquina. É possível adicionar novos dispositivos ao sistema, selecionar as métricas a serem monitoradas e configurar alertas para situações críticas. Além disso, a plataforma exibe relatórios detalhados e gráficos interativos que mostram o desempenho dos equipamentos em tempo real, permitindo uma análise aprofundada das condições operacionais e facilitando o planejamento da manutenção preventiva.

[Funcionamento IOT - IPTracker](https://youtu.be/D_cZk48Sjvc)

## 📁 Estrutura de pastas

Dentre os arquivos e pastas presentes na raiz do projeto, definem-se:

- **document**: aqui estão todos os documentos do projeto, incluindo o manual de instruções (se aplicável). Há também uma pasta denominada **assets** onde estão presentes arquivos gráficos.

- **src**: Todo o código fonte criado para o desenvolvimento do projeto, incluindo firmware, notebooks, backend e frontend, se aplicáveis.

- **README.md**: arquivo que serve como guia e explicação geral sobre o projeto (o mesmo que você está lendo agora).

## 🔧 Instalação

### 3. Guia de Instalação

#### 3.1 Instalação do Protótipo no Ambiente Físico
Esse tópico tem como objetivo instruir a instalação do dispositivo IoT no espaço físico, além de demonstrar a integração com a rede para o correto funcionamento.

**Pré-requisitos:**
- Protótipo IoT montado, contendo o case com sensores e atuadores.
- Ímãs de neodímio para fixar o dispositivo.
- Ambiente de instalação com conexão Wi-Fi ativa na faixa de 2.4 GHz.

1. **Fixar o dispositivo IoT:** Com o protótipo em mãos, escolha a máquina que será monitorada e acople-o utilizando o ímã de neodímio no espaço indicado do case. Certifique-se de posicioná-lo na orientação correta para evitar leituras imprecisas.
   *(Insira imagem ilustrativa aqui)*

2. **Fixar o sensor de temperatura:** Caso o sensor de temperatura esteja separado do case principal, fixe-o na superfície da máquina utilizando outro ímã de neodímio. Isso evita a dissipação de calor pelos materiais e garante medições precisas.
   *(Insira imagem ilustrativa aqui)*

**Dica:** Verifique a posição e orientação dos sensores ao instalar para evitar leituras errôneas.

---

#### 3.2 Especificações da Rede Wi-Fi
Essa seção especifica os detalhamentos da rede e os cuidados necessários para a instalação do protótipo no ambiente físico.

**Especificações do ESP32:**
- O ESP32 utiliza um módulo Wi-Fi integrado na frequência de **2.4 GHz**, compatível com a maioria dos roteadores domésticos.
- **Alcance do sinal Wi-Fi:**
  - Ambientes abertos: até 50 metros ou mais.
  - Ambientes internos com obstáculos: entre 10 e 30 metros, dependendo das barreiras presentes.

**Destaque do protótipo:**
- O dispositivo conta com uma antena integrada que amplia o alcance em ambientes internos para aproximadamente **45 metros**, garantindo maior estabilidade de conexão em condições adversas.

**Nota:** Para instruções detalhadas sobre a configuração da rede Wi-Fi, consulte o tópico [4.2 Configuração da Rede Wi-Fi](#4.2-configura%C3%A7%C3%A3o-da-rede-wi-fi).

---

#### 3.3 Pré-requisitos para a Instalação
Antes de iniciar a instalação e configuração do projeto, certifique-se de que os seguintes itens estão devidamente instalados e configurados:

**Bibliotecas Necessárias:**
- Baixe as bibliotecas no repositório oficial do projeto ([https://drive.google.com/drive/folders/1bXKmvdy8HNPJu4AkEc7ZlVl3c2uv-O0Z?usp=drive_link](https://drive.google.com/drive/folders/1bXKmvdy8HNPJu4AkEc7ZlVl3c2uv-O0Z?usp=drive_link)) e adicione na pasta `libraries` do Arduino IDE.

**Hardware:**
- **Módulo ESP32:** Modelo DOIT ESP32 DEVKIT V1.
- Sensores: BME280 (temperatura), HC-SR04 (distância), e sensor de vibração.
- Atuadores: LEDs, buzzer e display LCD.

Para mais informações sobre o fluxo de configuração, consulte o tópico [4. Guia de Configuração](#4-guia-de-configura%C3%A7%C3%A3o).

---

#### 3.4 Manual de Instruções
Para mais detalhes, acesse o [Manual de Instruções](https://docs.google.com/document/d/1R4nH4pD6cZUCy2cj_xZ3wi8QQ86DZDmhXUc8inS27fM/edit?tab=t.0) oficial do projeto.

---

## 🗃 Histórico de lançamentos

- 0.5.0 - 19/12/2024  
    Protótipo final do projeto, arquitetura refinada da solução e manual de instruções revisado.  
- 0.4.0 - 06/12/2024  
    Protótipo físico do projeto (online), manual de instruções com descarte dos componentes e arquitetura do protótipo.  
- 0.3.0 - 22/11/2024  
    Implementação dos protocolos MQTT e I2C e estruturação da arquitetura da solução.  
- 0.2.0 - 08/11/2024  
    Aprofundamento no entendimento do usuário, protótipo físico do projeto (offline) e levantamento dos requisitos não funcionais.  
- 0.1.0 - 25/10/2024  
    Entendimento de negócios e usuários, levantamento dos requisitos funcionais e protótipo inicial do projeto.

## 📋 Licença/License

<p xmlns:cc="http://creativecommons.org/ns#" xmlns:dct="http://purl.org/dc/terms/">
<a property="dct:title" rel="cc:attributionURL" href="https://github.com/Inteli-College/2024-2B-T12-IN04-G05">Projeto de Monitoramento IoT</a> by <a rel="cc:attributionURL dct:creator" property="cc:attributionName" href="https://github.com/Inteli-College/2024-2B-T12-IN04-G05">Ana Beggiato, Eduardo Rizk, Igor Sampaio Silva, Larissa Martins, Pedro Haouli, Raphael Felipe Silva, Ryan Botelho Gartlan</a> is licensed under <a href="https://creativecommons.org/licenses/by/4.0/?ref=chooser-v1" target="_blank"
#   I P T - I o T - l a b o r a t o r i o 
 
 
