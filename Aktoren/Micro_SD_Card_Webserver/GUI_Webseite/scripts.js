function getBaseURL() {
            return window.location.protocol + "//" + window.location.hostname + (window.location.port ? ":" + window.location.port : "");
        }

        function sendCocktail(cocktail) {
            const url = getBaseURL() + '/' + cocktail;
            fetch(url, {
                method: 'GET',
            })
            .then(response => {
                if (response.ok) {
                    console.log('Success:', cocktail);
                } else {
                    console.error('Error:', response.statusText);
                }
            })
            .catch((error) => console.error('Error:', error));
        }