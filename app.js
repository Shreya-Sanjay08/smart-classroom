// const firebaseConfig = {
//     apiKey: "AIzaSyC6onFdorEpwVGvdqWQ6p5tQo1u_nLkRGg",
//     authDomain: "attendance-41062.firebaseapp.com",
//     databaseURL: "https://attendance-41062-default-rtdb.asia-southeast1.firebasedatabase.app",
//     projectId: "attendance-41062",
//     storageBucket: "attendance-41062.firebasestorage.app",
//     messagingSenderId: "774004446167",
//     appId: "1:774004446167:web:31cba8253d0c72445e3192",
// };

// firebase.initializeApp(firebaseConfig);
// const db = firebase.database();


// // 🔹 Student Lookup
// async function fetchStudentAttendance() {
//     const uid = document.getElementById("student-uid").value.trim();
//     if (!uid) {
//         alert("Please enter a valid UID.");
//         return;
//     }

//     db.ref("attendance/" + uid).once("value").then((snapshot) => {
//         if (snapshot.exists()) {
//             const data = snapshot.val();
//             document.getElementById("student-details").innerHTML = `
//                 <p><strong>Name:</strong> ${data.name}</p>
//                 <p><strong>Attendance Percentage:</strong> ${data.percentage}%</p>
//                 <p><strong>Last Attendance:</strong> ${data.last_attendance}</p>
//                 ${data.percentage < 75 ? "<p class='warning'>⚠ Low Attendance! Please be cautious.</p>" : ""}
//             `;
//         } else {
//             alert("UID not found. Please try again.");
//         }
//     });
    

// // 🔹 Teacher Authentication
// function checkTeacherPassword() {
//     const password = prompt("Enter the teacher's password:");
//     const correctPassword = "teacher123"; // Change this to your actual password
//     if (password !== correctPassword) {
//         alert("Incorrect password! Redirecting to home.");
//         window.location.href = "index.html";
//     } else {
//         fetchAllStudents();
//     }
// }

// // 🔹 Fetch All Students for Teacher
// async function fetchAllStudents() {
//     const studentsTable = document.getElementById("student-data");
//     studentsTable.innerHTML = ""; // Clear existing data

//     db.ref("attendance").once("value").then((snapshot) => {
//         const studentsTable = document.getElementById("student-data");
//         studentsTable.innerHTML = ""; // Clear existing data
    
//         snapshot.forEach((childSnapshot) => {
//             const data = childSnapshot.val();
//             studentsTable.innerHTML += `<tr><td>${data.name}</td><td>${data.percentage}%</td></tr>`;
//         });
//     });
    
// 🔹 Firebase Configuration
const firebaseConfig = {
    apiKey: "AIzaSyC6onFdorEpwVGvdqWQ6p5tQo1u_nLkRGg",
    authDomain: "attendance-41062.firebaseapp.com",
    databaseURL: "https://attendance-41062-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "attendance-41062",
    storageBucket: "attendance-41062.firebasestorage.app",
    messagingSenderId: "774004446167",
    appId: "1:774004446167:web:31cba8253d0c72445e3192",
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);
const db = firebase.database();

// 🔹 Fetch Student Attendance
function fetchStudentAttendance() {
    const uid = document.getElementById("student-uid").value.trim();
    if (!uid) {
        alert("Please enter a valid name.");
        return;
    }

    const studentRef = db.ref("attendance/" + uid);

    studentRef.once("value", (snapshot) => {
        if (snapshot.exists()) {
            const data = snapshot.val();
            const percentage = ((data.attended_classes / data.total_classes) * 100).toFixed(2);
            
            document.getElementById("student-details").innerHTML = `
                <p><strong>Name:</strong> ${uid}</p>
                <p><strong>Attendance Percentage:</strong> ${percentage}%</p>
                <p><strong>Last Attendance:</strong> ${data.last_attendance}</p>
                ${percentage < 75 ? "<p class='warning'>⚠ Low Attendance! Please be cautious.</p>" : ""}
            `;
        } else {
            alert("Student not found. Please check the name.");
        }
    });
}


// 🔹 Teacher Authentication
function checkTeacherPassword() {
    const password = prompt("Enter the teacher's password:");
    const correctPassword = "teacher123"; // Change this to your actual password
    if (password !== correctPassword) {
        alert("Incorrect password! Redirecting to home.");
        window.location.href = "index.html";
    } else {
        fetchAllStudents();
    }
}

// 🔹 Fetch All Students (Real-Time Updates)
function fetchAllStudents() {
    const studentsTable = document.getElementById("student-data");
    studentsTable.innerHTML = ""; // Clear existing data

    const attendanceRef = db.ref("attendance");

    attendanceRef.on("value", (snapshot) => {
        studentsTable.innerHTML = ""; // Clear table before updating

        snapshot.forEach((childSnapshot) => {
            const studentName = childSnapshot.key;
            const data = childSnapshot.val();
            const percentage = ((data.attended_classes / data.total_classes) * 100).toFixed(2);

            studentsTable.innerHTML += `<tr><td>${studentName}</td><td>${percentage}%</td></tr>`;
        });
    });
}


// 🔹 Check if Firebase is Connected
db.ref(".info/connected").on("value", (snapshot) => {
    if (snapshot.val() === true) {
        console.log("✅ Connected to Firebase Realtime Database");
    } else {
        console.log("❌ Disconnected from Firebase");
    }
});

// db.ref(".info/connected").on("value", (snapshot) => {
//     if (snapshot.val() === true) {
//         console.log("✅ Connected to Firebase Realtime Database");
//     } else {
//         console.log("❌ Disconnected from Firebase");
//     }
// });
