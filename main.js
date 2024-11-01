document.addEventListener('DOMContentLoaded', () => {
  const form = document.getElementById('survey-form');

  form.addEventListener('submit', async function(event) {
    event.preventDefault();

    const formData = new FormData(form);

    const data = {};
    formData.forEach((value, key) => {
      data[key] = value;
    });

    let output = '';
    for (const key in data) {
      if (data.hasOwnProperty(key) && key !== 'name' && key !== 'email') {
        output += `${key.replace(/([A-Z])/g, ' $1')}: ${data[key]}\n`;
      }
    }
//	try {
//                // Update the file path to the location of your text file
//                const response = await fetch('file://Downloads/data.txt');
//                if (!response.ok) {
//                    throw new Error('Network response was not ok');
//                }
//                const content = await response.text();
//                output += "\nHeartBeat: ";
//		output += content;
//            } catch (error) {
//                console.error('Error reading file:', error);
//            }

    output += "\nĐây là đơn về sức khỏe của tôi thời gian gần đây bạn có thể cho tôi biết những vấn đề hay bệnh mà tôi đang gặp phải được không? Và cho tôi cách giải quyết chi tiết và hiệu quả !";
    // Send the output to ChatGPT API using CORS proxy
    try {
      const response = await fetch('https://cors-anywhere.herokuapp.com/https://api.openai.com/v1/chat/completions', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': 'Bearer sk-2jEbpeUKTaccHMEjtklxij1geT8UcEIDwL1E18nVpyT3BlbkFJ11xXZSHmsArkCZQmJlPA8dJmKcaNvf2Fy3FJD0EAUA' 
        },
        body: JSON.stringify({
          model: 'gpt-4o-mini',
          messages: [{ role: 'user', content: output}],
          max_tokens: 2000
        })
      });
      const result = await response.json();
      console.log(result); // Handle the response from ChatGPT
      document.getElementById("ketqua").textContent = result.choices[0].message.content;
 //     document.open();
    //  document.write('<pre>' + result.choices[0].message.content + '</pre>');
     // document.close();

    } catch (error) {
      console.error('Error:', error);
    }
  });
});
