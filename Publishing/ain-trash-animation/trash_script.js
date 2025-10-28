let filledAmount = 15
let trashIdCounter = 0
let isAnimating = false
let dbText = '48.20'

const fillButton = document.getElementById("fillButton")
const emptyButton = document.getElementById("emptyButton")
const trashCan = document.getElementById("trashCan")
const trashLid = document.getElementById("trashLid")
const fillLevel = document.getElementById("fillLevel")
const percentageText = document.getElementById("percentageText")
const statusText = document.getElementById("statusText")
const warningIcon = document.getElementById("warningIcon")
const odorLines = document.getElementById("odorLines")
const shakeLines = document.getElementById("shakeLines")
const fallingTrashContainer = document.getElementById("fallingTrashContainer")
const trashParticles = document.getElementById("trashParticles")


const progressFill = document.getElementById('progressFill');


function updateDisplay() {
  fillLevel.style.height = `${filledAmount}%`
  percentageText.textContent = dbText

  progressFill.style.width = filledAmount + '%'

  // Trigger scale animation
  percentageText.style.animation = "none"
  setTimeout(() => {
    percentageText.style.animation = "scaleIn 0.3s ease-out"
  }, 10)

  // Update status text
  if (filledAmount < 70) {
    statusText.textContent = `남은 용량: ${100 - filledAmount}%`
    statusText.classList.remove("full")
  } else {
    statusText.textContent = "쓰레기통을 비워주세요!"
    statusText.classList.add("full")
  }

  // Show/hide warning icon
  if (filledAmount >= 70) {
    warningIcon.style.display = "block"
  } else {
    warningIcon.style.display = "none"
  }

  // Show/hide odor lines
  if (filledAmount >= 70) {
    odorLines.style.display = "flex"
  } else {
    odorLines.style.display = "none"
  }

  // Update button state
  fillButton.disabled = filledAmount >= 100

  // Update trash particles
  updateTrashParticles()
}

function updateTrashParticles() {
  trashParticles.innerHTML = ""
  const particleCount = Math.floor(filledAmount / 10)

  for (let i = 0; i < particleCount; i++) {
    const particle = document.createElement("div")
    particle.className = "trash-particle"
    particle.style.left = `${15 + ((i * 23) % 70)}%`
    particle.style.top = `${10 + ((i * 17) % 80)}%`
    trashParticles.appendChild(particle)
  }
}

function createFallingTrash() {
  const trash = document.createElement("div")
  trash.className = "falling-trash"
  trash.id = `trash-${trashIdCounter++}`

  const xOffset = Math.random() * 20 - 10
  trash.style.setProperty("--x-offset", `${xOffset}px`)
  trash.style.animation = "fall 0.8s cubic-bezier(0.4, 0, 0.6, 1) forwards"

  fallingTrashContainer.appendChild(trash)

  setTimeout(() => {
    trash.remove()
  }, 800)
}

function fillTrashCan() {
  if (filledAmount >= 90 || isAnimating) {
    return
  }

  isAnimating = true

  // Open lid
  trashLid.classList.add("open")

  // Create falling trash
  createFallingTrash()

  // After trash falls
  setTimeout(() => {
    // Shake trash can
    trashCan.classList.add("shake")
    trashLid.classList.add("shake")
    shakeLines.style.display = "block"

    // Increase fill amount
    filledAmount = Math.min(filledAmount + 10, 100)
    updateDisplay()

    // Stop shaking
    setTimeout(() => {
      trashCan.classList.remove("shake")
      trashLid.classList.remove("shake")
      shakeLines.style.display = "none"
      isAnimating = false
    }, 500)
  }, 800)

  // Close lid
  setTimeout(() => {
    trashLid.classList.remove("open")
  }, 1200)
}

function emptyTrash() {
  filledAmount = 0
  updateDisplay()
}

// Event listeners
fillButton.addEventListener("click", fillTrashCan)
emptyButton.addEventListener("click", emptyTrash)

// Initialize display
updateDisplay()
