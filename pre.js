const self = {location: {href: 'blah'}};
function importScripts() {}
const performance = {
    now() {
        return Date.now();
    }
};

const crypto = {
    getRandomValues(array) {
        for (let i = 0; i < array.length; i++) {
            array[i] = (Math.random() * 256) | 0;
        }
    }
};
