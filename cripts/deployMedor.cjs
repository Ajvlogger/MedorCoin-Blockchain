// scripts/deployMedor.cjs
const { ethers } = require("hardhat");
const fs = require("fs");
const path = require("path");

async function main() {
    console.log("🚀 Starting MedorToken deployment sequence...");

    // Get deployer wallet
    const [deployer] = await ethers.getSigners();
    console.log(`👤 Deploying contracts with account: ${deployer.address}`);

    // Define initial setup parameters
    const initialReceiver = deployer.address;
    // Replace with actual active WBTC address on your target network
    const wbtcAddress = "0x2260FAC5E5542a773Aa44fBCfeDf7C1239180361"; 
    const initialRate = 100; // 100 MEDOR tokens per 1 WBTC

    // Load Contract Factory
    const MedorTokenFactory = await ethers.getContractFactory("MedorToken");
    
    // Deploy contract
    const medorToken = await MedorTokenFactory.deploy(initialReceiver, wbtcAddress, initialRate);
    await medorToken.waitForDeployment();

    const contractAddress = await medorToken.getAddress();
    console.log(`✅ MedorToken successfully deployed to: ${contractAddress}`);
}

main()
    .then(() => process.exit(0))
    .catch((error) => {
        console.error("❌ Deployment failed:", error);
        process.exit(1);
    });
