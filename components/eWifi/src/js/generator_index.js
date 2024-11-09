const options = document.querySelectorAll('.option');
const contents = document.querySelectorAll('.content');

options.forEach(option => {
    option.addEventListener('click', () => {
        options.forEach(opt => opt.classList.remove('active'));
        option.classList.add('active');

        contents.forEach(content => content.classList.remove('active'));
        const targetContent = document.getElementById(option.getAttribute('data-target'));
        targetContent.classList.add('active');
    });
});

function sendData(event, id) {
    event.preventDefault();
    const form = document.getElementById("form" + id);
    const formData = new URLSearchParams(new FormData(form)).toString();

    fetch(form.action, {
        method: 'POST',
        body: formData
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Error en la respuesta del servidor');
        }
        return response.text();
    })
    .then(data => {
        console.log("OK");
    })
    .catch(error => {
        console.error('Error:', error);
    });
}