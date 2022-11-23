use sakila;
-- 请列出所有商店的详细地址，显示商店id，商店地址，所在区域，所在城市，所在国家； 
SELECT store.store_id, address.address, address.district, city.city, country.country
	FROM store
    JOIN address ON store.address_id=address.address_id
    JOIN city ON city.city_id=address.city_id
    JOIN country ON city.country_id=country.country_id;
-- 哪些演员出演过影片《ROCKY WAR》？请列出他的first_name, last_name；
SELECT actor.first_name, actor.last_name
	FROM actor
	JOIN film_actor ON actor.actor_id=film_actor.actor_id
    JOIN film_text ON film_text.film_id=film_actor.film_id
    WHERE film_text.title="ROCKY WAR";
-- 找出租DVD花费最高的前5名，请列出他们的first_name, last_name和每个人花费的金额；
SELECT customer.first_name, customer.last_name, SUM(payment.amount)
	FROM customer
    JOIN rental ON rental.customer_id=customer.customer_id
    JOIN payment ON payment.rental_id=rental.rental_id
    GROUP BY customer.customer_id
    ORDER BY SUM(payment.amount) DESC
    LIMIT 5;
-- 哪个影片获得了总体最高的租金？请列出影片id、影片名、总租金；
SELECT film_text.film_id, film_text.title, SUM(payment.amount)
	FROM payment
    JOIN rental ON rental.rental_id=payment.rental_id
    JOIN inventory ON inventory.inventory_id=rental.inventory_id
    JOIN film_text ON film_text.film_id=inventory.film_id
    GROUP BY film_text.film_id
    ORDER BY SUM(payment.amount) DESC
    LIMIT 1;
-- 哪个演员出演的电影超过35部？ 请列出演员id、演员名、出演的电影数；
SELECT actor.actor_id, actor.first_name, actor.last_name, COUNT(*)
	FROM actor
    JOIN film_actor ON actor.actor_id=film_actor.actor_id
    GROUP BY actor.actor_id ORDER BY COUNT(*) DESC LIMIT 1;
-- 请找出没有租借过电影《TELEGRAPH VOYAGE》的顾客姓名；
SELECT DISTINCT first_name, last_name
	FROM customer
    WHERE customer_id IN (
		SELECT DISTINCT customer.customer_id
			FROM customer
			JOIN rental ON rental.customer_id=customer.customer_id
			JOIN inventory ON inventory.inventory_id=rental.inventory_id
			JOIN film_text ON film_text.film_id=inventory.film_id
			WHERE film_text.title="TELEGRAPH VOYAGE"
    );
-- 查询演过《ELEPHANT TROJAN》和《SPLASH GUMP》这两部电影的演员，列出其姓名；
SELECT DISTINCT actor.first_name, actor.last_name
	FROM actor
    JOIN film_actor ON film_actor.actor_id=actor.actor_id
    JOIN film_text ON film_actor.film_id=film_text.film_id
    WHERE film_text.title="ELEPHANT TROJAN" AND actor.actor_id IN (
		SELECT actor.actor_id
			FROM actor
				JOIN film_actor ON film_actor.actor_id=actor.actor_id
				JOIN film_text ON film_actor.film_id=film_text.film_id
                WHERE film_text.title="SPLASH GUMP"
    );
-- 统计每种类型的影片数，显示类型编号、类型名称、该类型影片数；
SELECT category.category_id, category.name
	FROM film_category
    JOIN category ON film_category.category_id=category.category_id
    GROUP BY category.category_id;
-- 有哪些影片是2个商店都有库存的？
SELECT DISTINCT film_text.title
	FROM inventory
    JOIN film_text ON film_text.film_id=inventory.film_id
	GROUP BY inventory.film_id
    HAVING COUNT(DISTINCT inventory.store_id) > 1;
-- 查询单次租借影片时间最长的6位客户，列出其first_name、last_name和当次租借时长；
SELECT customer.first_name, customer.last_name, TIMESTAMPDIFF(DAY, rental.rental_date, rental.return_date) as rental_days
	FROM rental
    JOIN customer ON customer.customer_id=rental.customer_id
    ORDER BY rental_days DESC
    LIMIT 6;


SELECT * FROM customer ORDER BY customer_id DESC LIMIT 1;
-- 在customer表中新增一条数据，注意customer表与其他表的关系；
INSERT INTO customer (
	store_id, first_name, last_name, email, address_id, active
) VALUES (
	(SELECT store_id FROM store ORDER BY store_id DESC LIMIT 1),
    "Chiro",
    "Liang",
    "Chiro2001@163.com",
    (SELECT address_id FROM address ORDER BY address_id DESC LIMIT 1),
    1
);
SELECT * FROM customer ORDER BY create_date DESC LIMIT 1;
-- 修改刚才在customer表中新增的那条数据；
UPDATE customer SET first_name="Lance" WHERE customer_id=(SELECT customer_id FROM customer ORDER BY create_date DESC LIMIT 1);
SELECT * FROM customer ORDER BY create_date DESC LIMIT 1;
-- 删除第11步新增的那条数据。
-- SELECT * FROM customer WHERE customer_id=(SELECT customer_id FROM customer ORDER BY create_date DESC LIMIT 1);
DELETE FROM customer WHERE customer_id=604;
SELECT * FROM customer ORDER BY create_date DESC LIMIT 1;

-- 思考题
-- 如果insert一条数据到actor表，但actor_id和已有数据重复，会发生什么？同学们请自己尝试一下，截图并分析原因。
SELECT * FROM actor WHERE actor_id=1;
INSERT INTO actor (actor_id, first_name, last_name) VALUES (1, "Chiro", "Liang");
SELECT * FROM actor WHERE actor_id=1;