-- view: actor_info
CREATE ALGORITHM = UNDEFINED DEFINER = `paul` @`localhost` SQL SECURITY INVOKER VIEW `sakila`.`actor_info` AS
select `a`.`actor_id` AS `actor_id`,
  `a`.`first_name` AS `first_name`,
  `a`.`last_name` AS `last_name`,
  group_concat(
    distinct concat(
      `c`.`name`,
      ': ',
      (
        select group_concat(
            `f`.`title`
            order by `f`.`title` ASC separator ', '
          )
        from (
            (
              `sakila`.`film` `f`
              join `sakila`.`film_category` `fc` on((`f`.`film_id` = `fc`.`film_id`))
            )
            join `sakila`.`film_actor` `fa` on((`f`.`film_id` = `fa`.`film_id`))
          )
        where (
            (`fc`.`category_id` = `c`.`category_id`)
            and (`fa`.`actor_id` = `a`.`actor_id`)
          )
      )
    )
    order by `c`.`name` ASC separator '; '
  ) AS `film_info`
from (
    (
      (
        `sakila`.`actor` `a`
        left join `sakila`.`film_actor` `fa` on((`a`.`actor_id` = `fa`.`actor_id`))
      )
      left join `sakila`.`film_category` `fc` on((`fa`.`film_id` = `fc`.`film_id`))
    )
    left join `sakila`.`category` `c` on((`fc`.`category_id` = `c`.`category_id`))
  )
group by `a`.`actor_id`,
  `a`.`first_name`,
  `a`.`last_name`;
CREATE ALGORITHM = UNDEFINED DEFINER = `paul` @`localhost` SQL SECURITY DEFINER VIEW `sakila`.`film_list` AS
select `sakila`.`film`.`film_id` AS `FID`,
  `sakila`.`film`.`title` AS `title`,
  `sakila`.`film`.`description` AS `description`,
  `sakila`.`category`.`name` AS `category`,
  `sakila`.`film`.`rental_rate` AS `price`,
  `sakila`.`film`.`length` AS `length`,
  `sakila`.`film`.`rating` AS `rating`,
  group_concat(
    concat(
      `sakila`.`actor`.`first_name`,
      _utf8mb4 ' ',
      `sakila`.`actor`.`last_name`
    ) separator ', '
  ) AS `actors`
from (
    (
      (
        (
          `sakila`.`category`
          left join `sakila`.`film_category` on(
            (
              `sakila`.`category`.`category_id` = `sakila`.`film_category`.`category_id`
            )
          )
        )
        left join `sakila`.`film` on(
          (
            `sakila`.`film_category`.`film_id` = `sakila`.`film`.`film_id`
          )
        )
      )
      join `sakila`.`film_actor` on(
        (
          `sakila`.`film`.`film_id` = `sakila`.`film_actor`.`film_id`
        )
      )
    )
    join `sakila`.`actor` on(
      (
        `sakila`.`film_actor`.`actor_id` = `sakila`.`actor`.`actor_id`
      )
    )
  )
group by `sakila`.`film`.`film_id`,
  `sakila`.`category`.`name`;
CREATE ALGORITHM = UNDEFINED DEFINER = `paul` @`localhost` SQL SECURITY DEFINER VIEW `sakila`.`sales_by_store` AS
select concat(`c`.`city`, _utf8mb4 ',', `cy`.`country`) AS `store`,
  concat(`m`.`first_name`, _utf8mb4 ' ', `m`.`last_name`) AS `manager`,
  sum(`p`.`amount`) AS `total_sales`
from (
    (
      (
        (
          (
            (
              (
                `sakila`.`payment` `p`
                join `sakila`.`rental` `r` on((`p`.`rental_id` = `r`.`rental_id`))
              )
              join `sakila`.`inventory` `i` on((`r`.`inventory_id` = `i`.`inventory_id`))
            )
            join `sakila`.`store` `s` on((`i`.`store_id` = `s`.`store_id`))
          )
          join `sakila`.`address` `a` on((`s`.`address_id` = `a`.`address_id`))
        )
        join `sakila`.`city` `c` on((`a`.`city_id` = `c`.`city_id`))
      )
      join `sakila`.`country` `cy` on((`c`.`country_id` = `cy`.`country_id`))
    )
    join `sakila`.`staff` `m` on((`s`.`manager_staff_id` = `m`.`staff_id`))
  )
group by `s`.`store_id`
order by `cy`.`country`,
  `c`.`city`;
CREATE ALGORITHM = UNDEFINED DEFINER = `paul` @`localhost` SQL SECURITY DEFINER VIEW `sakila`.`staff_list` AS
select `s`.`staff_id` AS `ID`,
  concat(`s`.`first_name`, _utf8mb4 ' ', `s`.`last_name`) AS `name`,
  `a`.`address` AS `address`,
  `a`.`postal_code` AS `zip code`,
  `a`.`phone` AS `phone`,
  `sakila`.`city`.`city` AS `city`,
  `sakila`.`country`.`country` AS `country`,
  `s`.`store_id` AS `SID`
from (
    (
      (
        `sakila`.`staff` `s`
        join `sakila`.`address` `a` on((`s`.`address_id` = `a`.`address_id`))
      )
      join `sakila`.`city` on((`a`.`city_id` = `sakila`.`city`.`city_id`))
    )
    join `sakila`.`country` on(
      (
        `sakila`.`city`.`country_id` = `sakila`.`country`.`country_id`
      )
    )
  );
CREATE DEFINER = `root` @`localhost` TRIGGER customer_create_date BEFORE
INSERT ON customer FOR EACH ROW
SET NEW.create_date = NOW();
INSERT INTO `sakila`.`customer` (
    `customer_id`,
    `store_id`,
    `first_name`,
    `last_name`,
    `email`,
    `address_id`,
    `active`,
    `create_date`,
    `last_update`
  )
VALUES (
    110619,
    1,
    "Chiro",
    "Liang",
    "Chiro2001@163.com",
    "翻斗大街翻斗花园2号楼1001室",
    1,
    null,
    null
  );
-- view
CREATE ALGORITHM = UNDEFINED DEFINER = `root` @`localhost` SQL SECURITY DEFINER VIEW `sakila`.`sales_by_city` AS
select concat(`c`.`city`, _utf8mb4 ',', `cy`.`country`) AS `city`,
  sum(`p`.`amount`) AS `total_sales`
from (
    (
      (
        (
          (
            (
              `sakila`.`payment` `p`
              join `sakila`.`rental` `r` on((`p`.`rental_id` = `r`.`rental_id`))
            )
            join `sakila`.`inventory` `i` on((`r`.`inventory_id` = `i`.`inventory_id`))
          )
          join `sakila`.`store` `s` on((`i`.`store_id` = `s`.`store_id`))
        )
        join `sakila`.`address` `a` on((`s`.`address_id` = `a`.`address_id`))
      )
      join `sakila`.`city` `c` on((`a`.`city_id` = `c`.`city_id`))
    )
    join `sakila`.`country` `cy` on((`c`.`country_id` = `cy`.`country_id`))
  )
 order by `cy`.`country`,
   `c`.`city`
  ;
select * from sales_by_city;


-- view: address_in_city
CREATE ALGORITHM = UNDEFINED DEFINER = `root` @`localhost` SQL SECURITY DEFINER VIEW `sakila`.`sales_in_country` AS
select country.country as `country`, COUNT(customer.customer_id) as `customers`, SUM(payment.amount) as `sales`, 
	SUM(payment.amount) / COUNT(customer.customer_id) as `ave`
from country
	join city on country.country_id=city.country_id
	join address on city.city_id=address.city_id
    join customer on customer.address_id=address.address_id
    join payment on customer.customer_id=payment.customer_id
    group by city.country_id
    order by SUM(payment.amount) desc;
select * from sales_in_country;

DELIMITER ;;
CREATE TRIGGER `ins_customer` BEFORE INSERT ON `customer` FOR EACH ROW BEGIN
    SET new.active = 1;
  END;;

INSERT INTO customer (
	store_id, first_name, last_name, email, address_id, active
) VALUES (
	(SELECT store_id FROM store ORDER BY store_id DESC LIMIT 1),
    "Chiro2",
    "Liang2",
    "Chiro2001@163.com",
    (SELECT address_id FROM address ORDER BY address_id DESC LIMIT 1),
    0
);


-- CREATE TRIGGER `upd_film` AFTER UPDATE ON `film` FOR EACH ROW BEGIN
--     IF (old.title != new.title) OR (old.description != new.description) OR (old.film_id != new.film_id)
--     THEN
--         UPDATE film_text
--             SET title=new.title,
--                 description=new.description,
--                 film_id=new.film_id
--         WHERE film_id=old.film_id;
--     END IF;
--   END;;


-- CREATE TRIGGER `del_film` AFTER DELETE ON `film` FOR EACH ROW BEGIN
--     DELETE FROM film_text WHERE film_id = old.film_id;
--   END;;

DELIMITER ;