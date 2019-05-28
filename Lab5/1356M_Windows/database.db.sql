BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "record_15693" (
	"cardID"	vchar,
	"booksID"	vchar,
	"borrow_time"	timestamp NOT NULL DEFAULT current_timestamp,
	"return_time"	timestamp DEFAULT null,
	FOREIGN KEY("booksID") REFERENCES "user"("booksID"),
	FOREIGN KEY("cardID") REFERENCES "user"("cardID")
);
CREATE TABLE IF NOT EXISTS "books_15693" (
	"booksID"	vchar,
	"book_name"	vchar,
	"author"	vchar,
	"press"	vchar,
	"count"	int,
	"residue"	int,
	"borrowed_days"	int,
	PRIMARY KEY("booksID")
);
CREATE TABLE IF NOT EXISTS "user_15693" (
	"cardID"	vchar,
	"name"	vchar,
	"gender"	vchar,
	"age"	int,
	"tele"	char(11),
	"status"	boolean,
	PRIMARY KEY("cardID")
);
INSERT INTO "record_15693" VALUES ('5A0C6B2C000104E0','E5EA6A2C000104E0','2019-04-29 21:09:19','2019-04-29 21:10:42');
INSERT INTO "books_15693" VALUES ('E5EA6A2C000104E0','dswa','dw','dwa',10,9,10);
INSERT INTO "user_15693" VALUES ('5A0C6B2C000104E0','小明','男',18,'15629109702',1);
COMMIT;
