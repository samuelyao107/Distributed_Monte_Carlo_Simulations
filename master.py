import pika
import json
import argparse

parser = argparse.ArgumentParser(description="Monte_Carlo option pricer — master coordinator")
parser.add_argument("--workers", type=int, default=4, help="Number of worker tasks to dispatch (default: 4)")
args = parser.parse_args()

num_workers = args.workers

connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
channel = connection.channel()
channel.queue_declare(queue='task_queue', durable=True)
channel.queue_declare(queue='result_queue', durable=True)

print(f"\n--- PHASE 1 : MAP (Distribution) --- [{num_workers} workers]")
for i in range(num_workers):
    task = {
        "S0_": 100.0,
        "K_": 100.0,
        "T_": 1.0,
        "r_": 0.05,
        "sigma_": 0.2,
        "num_paths_": 100000
    }
    channel.basic_publish(exchange='', routing_key='task_queue', body=json.dumps(task))
    print(f" [Master] Task {i} sent to workers.")

print("\n--- PHASE 2 : REDUCE (Aggregation) ---")
final_option_price = 0
results_received = 0

def on_result(ch, method, properties, body):
    global final_option_price, results_received

    result = json.loads(body)
    final_option_price += result['price']
    results_received += 1

    print(f" [Master] Partial result received: {result['price']:.4f}")
    ch.basic_ack(delivery_tag=method.delivery_tag)

    if results_received == num_workers:
        final_option_price /= num_workers
        print(f"\n [SUCCESS] REDUCE COMPLETE. Option price: {final_option_price:.4f}")
        ch.stop_consuming()

channel.basic_consume(queue='result_queue', on_message_callback=on_result)
channel.start_consuming()

connection.close()