import React, { useState, useEffect } from "react";
import "./App.css";
import Navbar from "./components/navbar";
import Dashboard from "./components/dashboard";
import Historico from "./components/historico";

function App() {

  return (
    <div>
      <Navbar />
      <main>
        <Dashboard />
        <Historico />
      </main>
    </div>
  );
}

export default App;
