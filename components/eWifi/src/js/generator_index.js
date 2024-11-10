const options = document.querySelectorAll('.content');
const submit_button = document.getElementById("submit_button");
const submit_card = document.getElementById('submit_card');
const select_mode = document.getElementById('select_mode');
const active_checkbox = document.getElementById('active_checkbox');
const div_for_hide = document.getElementById('div_for_hide');
const text_loading = document.getElementById("text_loading");
const loading_data = document.getElementById('loading_data');
const input_generator = document.getElementById("input_generator");

select_mode.addEventListener('change',()=>{
    document.querySelectorAll('.modes').forEach(item => item.classList.add('d-none'));
    if(select_mode.value == "-1")
        div_for_hide.classList.add('d-none')
    else{
        document.getElementById('mode'+select_mode.value).classList.remove("d-none");
        div_for_hide.classList.remove('d-none')
    }
});


function set_form_color(active_or_error){
    submit_button.classList.add(active_or_error);
    submit_card.classList.add(active_or_error);
    

    setTimeout(function() {
        submit_button.classList.remove(active_or_error);
        submit_card.classList.remove(active_or_error);
    }, 1000); 
}



options.forEach(option => {
    option.addEventListener('click', () => {
        options.forEach(opt => opt.classList.remove('active'));
        option.classList.add('active');
    });
});


function GetData(generator){
    params = new URLSearchParams();
    params.append("generator", generator);
    loading_data.classList.remove('d-none')
    submit_card.classList.add('d-none');
    text_loading.style.color = "black"
    fetch('./get_generator_data', {
        method: 'POST',
        body: params.toString()
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Error en la respuesta del servidor');
        }
        return response.json();
    })
    .then(data => {
        console.log("OK");
        loading_data.classList.add('d-none');
        submit_card.classList.remove('d-none');
        input_generator.value = generator;
        select_mode.value = data.mode
        select_mode.dispatchEvent(new Event('change'));
    })
    .catch(error => {
        text_loading.style.color= "red";
        text_loading.innerHTML="Error at Fetch";
        console.error('Error:', error);
    });
}

GetData(0);

function sendData(event) {
    event.preventDefault();
    const form_element = document.getElementById("main_form");
    const data = new URLSearchParams();
    const form = new FormData(form_element);
    var list;
    switch (select_mode.value) {
        case 0:
            list = ['frecuency']
            break;
        
        case 1:
            list = ['min_frecuency','max_frecuency','drift']
            break;

        case 2:
            list = ['analog_gpio']
            break;
        default:
            list = []
            break;
    }
    list.push('mode','input_gpio');


    list.forEach(item => {
        data.append(item,form.get(item));
    })
    
    
    fetch(form.action, {
        method: 'POST',
        body: data.toString()
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Error en la respuesta del servidor');
        }
        return response.text();
    })
    .then(data => {
        set_form_color('active');
        console.log("OK");
    })
    .catch(error => {
        set_form_color('error');
        console.error('Error:', error);
    });
}