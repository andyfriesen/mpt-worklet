const self = {location: {href: 'blah'}};
const importScripts = () => {};
const performance = {
    now: () => Date.now()
}

const crypto = {
    getRandomValues(array) {
        for (let i = 0; i < array.length; i++)
            array[i] = (Math.random()*256)|0;
    }
};
